#include <sys/filio.h>
#include "config.h"
#include "io.h"
#include "atl.h"
#ifndef MODULE
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

#include "unix_defs.h"
#include <gen_thread.h>
#else

#include "kernel/katl.h"
#include "kernel/library.h"
#include "kernel/kernel_defs.h"
#include "assert.h"
#endif
#include "atom_internal.h"

#define MAXDATASIZE 100
#include "tclHash.h"

/* opaque type for atom server handle */
typedef struct _atom_server {
    int sockfd, addr_len;
    struct hostent *he;
    struct sockaddr_in their_addr;
    int flags;
    char *server_id;
    int get_send_format_id;
    int prov_use_format_id;
    atom_cache_type cache_style;
    thr_mutex_t hash_lock;
    Tcl_HashTable string_hash_table;
    Tcl_HashTable value_hash_table;
    int next_atom_id;
} atom_server_struct;

static char *atom_server_host = NULL;

static void
set_blocking(as, block)
atom_server as;
int block;
{
    if (block && ((as->flags & O_NONBLOCK) == 0)) {
	return;			/* already blocking */
    }
    if (!block && ((as->flags & O_NONBLOCK) == O_NONBLOCK)) {
	return;			/* already non-blocking */
    }
    if (block) {
	as->flags &= (~O_NONBLOCK);
    } else {
	as->flags |= O_NONBLOCK;
    }
    if (fcntl(as->sockfd, F_SETFL, as->flags) < 0) {
	perror("fcntl");
	exit(1);
    }
}

static void
handle_unexpected_msg(as, msg)
atom_server as;
char *msg;
{
    switch (msg[0]) {
    case 'E':{
	    Tcl_HashEntry *entry = NULL;
	    char *str;
	    int atom;
	    atom = strtol(&msg[1], &str, 10);
	    str++;
	    thr_mutex_lock(as->hash_lock);
	    entry = Tcl_FindHashEntry(&as->string_hash_table, str);
	    if (entry != NULL) {
		send_get_atom_msg_ptr atom_entry =
		(send_get_atom_msg_ptr) Tcl_GetHashValue(entry);
		if ((atom_entry != NULL) && (atom_entry->atom != atom)) {
		    printf("Warning:  Atom use inconsistency.\n");
		    printf("\tThis program associates the string \"%s\" with atom value %d\n",
			   str, atom_entry->atom);
		    printf("\tOther programs use the atom value %d\n", atom);
		}
	    }
	    entry = Tcl_FindHashEntry(&as->value_hash_table, (char *) atom);
	    if (entry != NULL) {
		send_get_atom_msg_ptr atom_entry =
		(send_get_atom_msg_ptr) Tcl_GetHashValue(entry);
		if ((atom_entry != NULL) &&
		    (strcmp(atom_entry->atom_string, str) != 0)) {
		    printf("Warning:  Atom use inconsistency.\n");
		    printf("\tThis program associates the string \"%s\" with atom value %d\n",
			   atom_entry->atom_string, atom_entry->atom);
		    printf("\tOther programs associate the string \"%s\" with that value\n", str);
		}
		printf("Atom cache inconsistency, tried to associate value %d with string \"%s\"\n	Previous association was string \"%s\"\n",
		       atom, str, atom_entry->atom_string);
	    }
	    thr_mutex_unlock(as->hash_lock);
	    break;
	}
    default:
	printf("Warning: Got an unexpected message \"%s\"\n", msg);
    }
}

static
 send_get_atom_msg_ptr
enter_atom_into_cache(as, msg)
atom_server as;
send_get_atom_msg_ptr msg;
{
    int new;
    char *str;
    send_get_atom_msg_ptr stored;
    Tcl_HashEntry *entry = NULL;

    if ((msg->atom_string == NULL) || (msg->atom == -1))
	return msg;
    str = strdup(msg->atom_string);
    stored = (send_get_atom_msg_ptr) malloc(sizeof(send_get_atom_msg));
    stored->atom_string = str;
    stored->atom = msg->atom;

    /* enter into string hash table */
    if (gen_thr_initialized()) {
	if (as->hash_lock == NULL) {
	    as->hash_lock = thr_mutex_alloc();
	}
	thr_mutex_lock(as->hash_lock);
    }
    entry = Tcl_CreateHashEntry(&as->string_hash_table, str, &new);
    if (!new) {
	/* already inserted by someone else */
	free(stored);
	free(str);
	thr_mutex_unlock(as->hash_lock);
	return (send_get_atom_msg_ptr) Tcl_GetHashValue(entry);
    }
    Tcl_SetHashValue(entry, stored);
    /* enter into value hash table */
    entry = Tcl_CreateHashEntry(&as->value_hash_table,
				(char *) stored->atom, &new);
    if (!new) {
	printf("Serious internal error in atom cache.  Duplicate value hash entry.\n");
	thr_mutex_unlock(as->hash_lock);
	exit(1);
    }
    Tcl_SetHashValue(entry, stored);
    thr_mutex_unlock(as->hash_lock);
    return (send_get_atom_msg_ptr) Tcl_GetHashValue(entry);
}

extern
void
set_string_and_atom(as, str, atom)
atom_server as;
char *str;
atom_t atom;
{
    send_get_atom_msg tmp_value;
    Tcl_HashEntry *entry = NULL, *entry2 = NULL;
    int numbytes, len;
    char buf[MAXDATASIZE];
    int addr_len = sizeof(struct sockaddr);
    printf("Set string and atom called with %s, %d\n", str, atom);

    thr_mutex_lock(as->hash_lock);
    entry = Tcl_FindHashEntry(&as->string_hash_table, str);
    if (entry != NULL) {
	send_get_atom_msg_ptr atom_entry =
	(send_get_atom_msg_ptr) Tcl_GetHashValue(entry);
	if ((atom_entry != NULL) && (atom_entry->atom != atom)) {
	    printf("Atom cache inconsistency, tried to associate string \"%s\" with value %d\n	Previous association was value %d\n",
		   str, atom, atom_entry->atom);
	    return;
	}
    }
    entry2 = Tcl_FindHashEntry(&as->value_hash_table, (char *) atom);
    if (entry2 != NULL) {
	send_get_atom_msg_ptr atom_entry =
	(send_get_atom_msg_ptr) Tcl_GetHashValue(entry2);
	if ((atom_entry != NULL) &&
	    (strcmp(atom_entry->atom_string, str) != 0)) {
	    printf("Atom cache inconsistency, tried to associate value %d with string \"%s\"\n	Previous association was string \"%s\"\n",
		   atom, str, atom_entry->atom_string);
	    return;
	}
    }
    tmp_value.atom = atom;
    tmp_value.atom_string = str;
    enter_atom_into_cache(as, &tmp_value);
    thr_mutex_unlock(as->hash_lock);

    sprintf(buf, "A%d %s", atom, str);
    len = strlen(buf);
    set_blocking(as, 0);	/* set server fd nonblocking */
    if ((numbytes = sendto(as->sockfd, buf, len, 0,
			   (struct sockaddr *) &(as->their_addr), sizeof(struct sockaddr))) == -1) {
	perror("sendto");
	exit(1);
    }
    if ((numbytes = recvfrom(as->sockfd, buf, MAXDATASIZE - 1, 0,
	      (struct sockaddr *) &(as->their_addr), &addr_len)) != -1) {
	/* actually got a message back ! */
	buf[numbytes] = 0;
	handle_unexpected_msg(as, buf);
    }
}

extern
 atom_t
atom_from_string(as, str)
atom_server as;
char *str;
{
    send_get_atom_msg tmp_rec;
    Tcl_HashEntry *entry = NULL;
    int numbytes, len;
    char buf[MAXDATASIZE];

    if (gen_thr_initialized()) {
	if (as->hash_lock == NULL) {
	    as->hash_lock = thr_mutex_alloc();
	}
	thr_mutex_lock(as->hash_lock);
    }
    entry = Tcl_FindHashEntry(&as->string_hash_table, str);
    thr_mutex_unlock(as->hash_lock);
    if (entry == NULL) {

	set_blocking(as, 1);	/* set server fd blocking */
	len = strlen(str) + 2;
	buf[0] = 'S';		/* string message */
	strncpy(&buf[1], str, sizeof(buf) - 1);
	if (sendto(as->sockfd, buf, len, 0,
		   (struct sockaddr *) &(as->their_addr),
		   sizeof(struct sockaddr)) == -1) {
	    perror("sendto");
	}
	buf[0] = 0;
	while (buf[0] != 'N') {
	    if ((numbytes = recvfrom(as->sockfd, buf, MAXDATASIZE - 1, 0,
				   (struct sockaddr *) &(as->their_addr),
				     &(as->addr_len))) == -1) {
		perror("recvfrom");
		exit(1);
	    }
	    buf[numbytes] = 0;
	    if (buf[0] != 'N')
		handle_unexpected_msg(as, buf);
	}

	tmp_rec.atom_string = str;
	tmp_rec.atom = atoi(&buf[1]);
	enter_atom_into_cache(as, &tmp_rec);
	return tmp_rec.atom;
    } else {
	return ((send_get_atom_msg_ptr) Tcl_GetHashValue(entry))->atom;
    }
}

extern
char *
string_from_atom(as, atom)
atom_server as;
atom_t atom;
{
    send_get_atom_msg tmp_rec;
    send_get_atom_msg_ptr stored;
    Tcl_HashEntry *entry = NULL;
    int numbytes;
    char buf[MAXDATASIZE];

    if (gen_thr_initialized()) {
	if (as->hash_lock == NULL) {
	    as->hash_lock = thr_mutex_alloc();
	}
	thr_mutex_lock(as->hash_lock);
    }
    entry = Tcl_FindHashEntry(&as->value_hash_table, (char *) atom);
    thr_mutex_unlock(as->hash_lock);

    if (entry == NULL) {
	sprintf(buf, "N%d", atom);
	if (sendto(as->sockfd, buf, strlen(buf), 0,
		   (struct sockaddr *) &(as->their_addr),
		   sizeof(struct sockaddr)) == -1) {
	    perror("send");
	}
	set_blocking(as, 1);	/* set server fd blocking */
	buf[0] = 0;
	while (buf[0] != 'S') {
	    if ((numbytes = recvfrom(as->sockfd, buf, MAXDATASIZE - 1, 0,
				   (struct sockaddr *) &(as->their_addr),
				     &(as->addr_len))) == -1) {
		perror("recv");
		exit(1);
	    }
	    buf[numbytes] = 0;
	    if (buf[0] != 'S')
		handle_unexpected_msg(as, buf);
	}

	if (buf[1] == 0)
	    return NULL;

	tmp_rec.atom_string = &buf[1];
	tmp_rec.atom = atom;

	stored = enter_atom_into_cache(as, &tmp_rec);
    } else {
	stored = (send_get_atom_msg_ptr) Tcl_GetHashValue(entry);
    }
    if (stored->atom_string != NULL) {
	return strdup(stored->atom_string);
    } else {
	return NULL;
    }
}

extern
char *
get_server_id(as)
atom_server as;
{
    return as->server_id;
}

atom_server
init_atom_server(cache_style)
atom_cache_type cache_style;
{
    atom_server as = (atom_server) malloc(sizeof(atom_server_struct));

    if (atom_server_host == NULL) {	/* environment override */
	atom_server_host = getenv("ATOM_SERVER_HOST");
    }
    if (atom_server_host == NULL) {
	atom_server_host = ATOM_SERVER_HOST;	/* from configure */
    }
    as->server_id = atom_server_host;

    Tcl_InitHashTable(&as->string_hash_table, TCL_STRING_KEYS);
    Tcl_InitHashTable(&as->value_hash_table, TCL_ONE_WORD_KEYS);
    as->hash_lock = thr_mutex_alloc();
    as->next_atom_id = 1000;
    as->cache_style = cache_style;

    if ((as->he = gethostbyname(atom_server_host)) == NULL) {
	perror("gethostbyname");
	exit(1);
    }
    if ((as->sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
	perror("socket");
	exit(1);
    }
    as->flags = fcntl(as->sockfd, F_GETFL);

    as->their_addr.sin_family = AF_INET;
    as->their_addr.sin_port = htons(PORT);
    as->their_addr.sin_addr = *((struct in_addr *) as->he->h_addr);
    memset(&(as->their_addr.sin_zero), '\0', 8);

    return as;
}
