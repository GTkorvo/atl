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
#include <sys/types.h>
#ifndef HAVE_WINDOWS_H
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#else
#include <windows.h>
#include <winsock.h>
#endif
#include <fcntl.h>

#include "unix_defs.h"
#include <gen_thread.h>
#include "cercs_env.h"
#else
/* this is just dummied up */
struct sockaddr_in { };

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
    int sockfd;
    int tcp_fd;
    int use_tcp;
    struct hostent *he;
    struct sockaddr_in their_addr;
    int flags;
    char *server_id;
    thr_mutex_t hash_lock;
    Tcl_HashTable string_hash_table;
    Tcl_HashTable value_hash_table;
} atom_server_struct;

static char *atom_server_host = NULL;
static int establish_server_connection(atom_server as, int do_fallback);

#ifndef O_NONBLOCK
#define O_NONBLOCK 0x80
#endif
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
#ifndef MODULE
#ifndef HAVE_WINDOWS_H
    if (fcntl(as->sockfd, F_SETFL, as->flags) < 0) {
	perror("fcntl");
	exit(1);
    }
    fcntl(as->tcp_fd, F_SETFL, as->flags);
#else
    if (ioctlsocket(as->sockfd, FIONBIO, (unsigned long*)!block) != 0) {
	perror("ioctlsocket");
	exit(1);
    }
    ioctlsocket(as->tcp_fd, FIONBIO, (unsigned long*)!block);
#endif
#endif
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
		    printf("\tThis program associates the string \"%s\" with atom value %d, %x, '%c%c%c%c'\n",
			   str, atom_entry->atom, atom_entry->atom,
			   ((char*)&atom_entry->atom)[0],
			   ((char*)&atom_entry->atom)[1],
			   ((char*)&atom_entry->atom)[2],
			   ((char*)&atom_entry->atom)[3]);
		    printf("\tOther programs use the atom value %d, %x, '%c%c%c%c'\n", atom,
			   atom, ((char*)&atom)[0], ((char*)&atom)[1], 
			((char*)&atom)[2], ((char*)&atom)[3]
			);
		}
	    }
	    entry = Tcl_FindHashEntry(&as->value_hash_table, (char *) atom);
	    if (entry != NULL) {
		send_get_atom_msg_ptr atom_entry =
		(send_get_atom_msg_ptr) Tcl_GetHashValue(entry);
		if ((atom_entry != NULL) &&
		    (strcmp(atom_entry->atom_string, str) != 0)) {
		    printf("Warning:  Atom use inconsistency.\n");
		    printf("\tThis program associates the string \"%s\" with atom value %d, %x, '%c%c%c%c'\n",
			   atom_entry->atom_string, atom_entry->atom,
			   atom_entry->atom, 
			   ((char*)&atom_entry->atom)[0],
			   ((char*)&atom_entry->atom)[1],
			   ((char*)&atom_entry->atom)[2],
			   ((char*)&atom_entry->atom)[3]);
		    printf("\tOther programs associate the string \"%s\" with that value\n", str);
		}
		printf("Atom cache inconsistency, tried to associate value %d %x, '%c%c%c%c' with string \"%s\"\n	Previous association was string \"%s\"\n",
		       atom, atom, ((char*)&atom)[0], ((char*)&atom)[1], 
		       ((char*)&atom)[2], ((char*)&atom)[3], str, atom_entry->atom_string);
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
    unsigned char buf[MAXDATASIZE];
    int addr_len = sizeof(struct sockaddr);

    thr_mutex_lock(as->hash_lock);
    entry = Tcl_FindHashEntry(&as->string_hash_table, str);
    if (entry != NULL) {
	send_get_atom_msg_ptr atom_entry =
	(send_get_atom_msg_ptr) Tcl_GetHashValue(entry);
	if ((atom_entry != NULL) && (atom_entry->atom != atom)) {
	    printf("Atom cache inconsistency, tried to associate string \"%s\" with value %d, %x, '%c%c%c%c'\n	Previous association was value %d, %x, '%c%c%c%c'\n", 

		   str, atom, atom, ((char*)&atom)[0], ((char*)&atom)[1], 
		   ((char*)&atom)[2], ((char*)&atom)[3], atom_entry->atom,
		   atom_entry->atom, ((char*)&atom_entry->atom)[0],
		   ((char*)&atom_entry->atom)[1],
		   ((char*)&atom_entry->atom)[2],
		   ((char*)&atom_entry->atom)[3]);
	    thr_mutex_unlock(as->hash_lock);
	    return;
	}
    }
    entry2 = Tcl_FindHashEntry(&as->value_hash_table, (char *) atom);
    thr_mutex_unlock(as->hash_lock);
    if (entry2 != NULL) {
	send_get_atom_msg_ptr atom_entry =
	(send_get_atom_msg_ptr) Tcl_GetHashValue(entry2);
	if ((atom_entry != NULL) &&
	    (strcmp(atom_entry->atom_string, str) != 0)) {
	    printf("Atom cache inconsistency, tried to associate value %d, %x, '%c%c%c%c' with string \"%s\"\n	Previous association was string \"%s\"\n",
		   atom, atom, ((char*)&atom)[0], ((char*)&atom)[1], 
		   ((char*)&atom)[2], ((char*)&atom)[3], str, 
		   atom_entry->atom_string);
	    return;
	}
    }
    tmp_value.atom = atom;
    tmp_value.atom_string = str;
    enter_atom_into_cache(as, &tmp_value);

#ifndef MODULE
    sprintf((char *)&buf[1], "A%d %s", atom, str);
    len = strlen((char*)&buf[1]);
    if (as->use_tcp) {
	set_blocking(as, 1);
	buf[0] = len;
	establish_server_connection(as, 1);
	if ((numbytes = write(as->tcp_fd, buf, len+1)) != len +1) {
	    close(as->tcp_fd);
	    return;
	}
	set_blocking(as, 0);
	if (read(as->tcp_fd, buf, 1) != 1) {
	    return;
	}
	if (read(as->tcp_fd, &buf[1], buf[0]) != buf[0]) {
	    return;
	}
	buf[buf[0]+1] = 0;
	handle_unexpected_msg(as, &buf[1]);
    } else {
	set_blocking(as, 0);	/* set server fd nonblocking */
	if ((numbytes = sendto(as->sockfd, &buf[1], len, 0,
			       (struct sockaddr *) &(as->their_addr), sizeof(struct sockaddr))) == -1) {
	    perror("sendto");
	    exit(1);
	}
	if ((numbytes = recvfrom(as->sockfd, &buf[1], MAXDATASIZE - 1, 0,
				 (struct sockaddr *) &(as->their_addr), &addr_len)) != -1) {
	    /* actually got a message back ! */
	    buf[numbytes+1] = 0;
	    handle_unexpected_msg(as, &buf[1]);
	}
    }
#endif
}

static int atom_server_verbose = 0;

static int
fill_hostaddr(void *addr, char *hostname)
{
#ifdef MODULE
    struct hostent *host_addr;

    if ((host_addr = lookup_name(hostname)) == NULL) {
	return 0;
    }
    memcpy(addr, host_addr->h_addr, host_addr->h_length);
    DFreeMM((addrs_t)host_addr->h_name);
    DFreeMM((addrs_t)host_addr);
    return 1;
#else
    struct hostent *host_addr;
    
    host_addr = gethostbyname(hostname);
    if (host_addr == NULL) {
	unsigned long addr = inet_addr(hostname);
	if (addr == -1) {
	    /* 
	     *  not translatable as a hostname or 
	     * as a dot-style string IP address
	     */
	    return 0;
	}
	assert(sizeof(int) == sizeof(struct in_addr));
	*((int*)addr) = (int)addr;
    } else {
	memcpy(addr, host_addr->h_addr, host_addr->h_length);
    }
    return 1;
#endif
}

static int
establish_server_connection(as, do_fallback)
atom_server as;
int do_fallback;
{
    int sock;
    int junk_errno;
    char *junk_str;
    int ret;
#ifndef MODULE
    int delay_value = 1;
#else
    struct socket *socket;	
#endif
    char ping_char = 0;


    if (atom_server_verbose == -1) {
	if (getenv("ATOM_SERVER_VERBOSE") == NULL) {
	    atom_server_verbose = 0;
	} else {
	    atom_server_verbose = 1;
	}
    }
    if ((as->tcp_fd == -1) || 
	(write(as->tcp_fd, &ping_char, 1) != 1)) {
	/* reestablish connection, name_str is the machine name */
	struct sockaddr_in sock_addr;

#ifdef MODULE
        if ((sock = sock_create(AF_INET, SOCK_STREAM, 0, &socket)) < 0) {
	    printk("Failed to create socket for ATL atom server connection.  Not enough File Descriptors?\n");
	    return 0;
	}
	//make_rt_sock(socket);
	sock = get_pseudofd(socket, 
	    (struct socket **)current->artemis->fdartemis);
#else
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	    fprintf(stderr, "Failed to create socket for ATL atom server connection.  Not enough File Descriptors?\n");
	    return 0;
	}
#endif
	
	sock_addr.sin_family = AF_INET;
		
	if (fill_hostaddr(&sock_addr.sin_addr, atom_server_host) == 0) {
	    fprintf(stderr, "Unknown Host \"%s\" specified as ATL atom server.\n",
		    atom_server_host);
	    return 0;
	}
	sock_addr.sin_port = htons(TCP_PORT);

	if (atom_server_verbose) {
	    printf("Trying connection to atom server on %s ...  ",
		   atom_server_host);
	}
#ifdef MODULE
        if (socket->ops->connect(socket, (struct sockaddr *) &sock_addr,
		    sizeof sock_addr, O_RDWR ) < 0) {
#else
	if (connect(sock, (struct sockaddr *) &sock_addr,
		    sizeof sock_addr) < 0) {
#endif

	    if (atom_server_verbose) {
		printf("failed\n");
	    }
	    if (!do_fallback) return 0;

	    /* fallback */
#ifdef MODULE
            if ((sock = sock_create(AF_INET, SOCK_STREAM, 0, &socket)) < 0) {
		printk("Failed to create socket for ATL atom server connection.  Not enough File Descriptors?\n");
	       return 0;
	    }
	    //make_rt_sock(socket);
	    sock = get_pseudofd(socket, 
		(struct socket **)current->artemis->fdartemis);
#else
	    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "Failed to create socket for ATL atom server connection.  Not enough File Descriptors?\n");
	       return 0;
	    }
#endif
	    atom_server_host = "atomhost.cercs.gatech.edu";
	    sock_addr.sin_family = AF_INET;
	    if (fill_hostaddr(&sock_addr.sin_addr, atom_server_host) == 0){
		fprintf(stderr, "Unknown Host \"%s\" specified as ATL atom server.\n",
			atom_server_host);
		return 0;
	    }
	    sock_addr.sin_port = htons(TCP_PORT);
	    if (atom_server_verbose) {
		printf("Trying fallback connection to atom server on %s ...  ",
		       atom_server_host);
	    }
#ifdef MODULE
            if (socket->ops->connect(socket, (struct sockaddr *) &sock_addr,
			sizeof sock_addr, O_RDWR) < 0) {
#else
	    if (connect(sock, (struct sockaddr *) &sock_addr,
			sizeof sock_addr) < 0) {
#endif
		fprintf(stderr, "Failed to connect to primary or fallback atom servers.\n");
	        return 0;
	    }
	}
	if (atom_server_verbose) {
	    printf("succeeded\n");
	}
#ifndef MODULE
	setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *) &delay_value,
		   sizeof(delay_value));
#endif
	as->tcp_fd = sock;
	/* 
	 * ignore SIGPIPE's  (these pop up when ports die.  we catch the 
	 * failed writes) 
	 */
#ifdef SIGPIPE
	signal(SIGPIPE, SIG_IGN);
#endif
	if (ret != 1) {
	    /* 
	     * got a server, but it was unacceptable in some way, 
	     * didn't give us a magic number or had restarted since 
	     *  last time. 
	     */
	    return 0;
	}
    }
    return 1;
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
    unsigned char buf[MAXDATASIZE];

    if (gen_thr_initialized()) {
	if (as->hash_lock == NULL) {
	    as->hash_lock = thr_mutex_alloc();
	}
	thr_mutex_lock(as->hash_lock);
    }
    entry = Tcl_FindHashEntry(&as->string_hash_table, str);
    thr_mutex_unlock(as->hash_lock);
    if (entry == NULL) {
#ifndef MODULE
	establish_server_connection(as, 1);
	set_blocking(as, 1);	/* set server fd blocking */
	len = strlen(str) + 2;
	buf[1] = 'S';		/* string message */
	strncpy((char*)&buf[2], str, sizeof(buf) - 1);
	buf[0] = len;
	printf("Writing %d chars, %s\n", buf[0], &buf[1]);
	if (write(as->tcp_fd, buf, len+1) == -1) {
	    perror("write");
	    close(as->tcp_fd);
	}
	buf[1] = 0;
	while (buf[1] != 'N') {
	    if ((numbytes = read(as->tcp_fd, buf, 1)) == -1) {
		perror("read");
		return -1;
	    }
	    if ((numbytes = read(as->tcp_fd, &buf[1], buf[0])) != buf[0]) {
		perror("read2");
		return -1;
	    }
	    buf[numbytes+1] = 0;
	    if (buf[1] != 'N')
		handle_unexpected_msg(as, &buf[1]);
	}

	tmp_rec.atom_string = str;
	tmp_rec.atom = atoi((char*)&buf[2]);
	/* enter into cache only if we got an answer */
#else
	tmp_rec.atom = -1;
#endif
	if (tmp_rec.atom != -1) {
	    enter_atom_into_cache(as, &tmp_rec);
	} else {
	    printf("No atom value found for string\"%s\".\n", str);
	}
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
#ifndef MODULE
	sprintf(&buf[1], "N%d", atom);
	establish_server_connection(as, 1);
	buf[0] = strlen(&buf[1]);
	if (write(as->tcp_fd, buf, buf[0]+1) != buf[0] + 1) {
	    perror("write");
	    return NULL;
	}
	set_blocking(as, 1);	/* set server fd blocking */
	buf[1] = 0;
	while (buf[1] != 'S') {
	    if ((numbytes = read(as->tcp_fd, buf, 1)) == -1) {
		perror("read");
		return NULL;
	    }
	    if ((numbytes = read(as->tcp_fd, &buf[1], buf[0])) != buf[0]) {
		perror("read2");
		return NULL;
	    }
	    buf[numbytes+1] = 0;
	    if (buf[1] != 'S')
		handle_unexpected_msg(as, &buf[1]);
	}

	if (buf[2] == 0)
	    return NULL;

	tmp_rec.atom_string = &buf[2];
	tmp_rec.atom = atom;

	stored = enter_atom_into_cache(as, &tmp_rec);
#else
	return NULL;
#endif	
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

#ifdef HAVE_WINDOWS_H
/* Winsock init stuff, ask for ver 1.1 */
static WORD wVersionRequested = MAKEWORD(1, 1);
static WSADATA wsaData;

static void
nt_socket_init_func()
{
    int nErrorStatus;
    nErrorStatus = WSAStartup(wVersionRequested, &wsaData);
    if (nErrorStatus != 0) {
	fprintf(stderr, "Could not initialize windows socket library!");
	WSACleanup();
	exit(-1);
    }
}
#else
static void nt_socket_init_func(){}
#endif

atom_server
init_atom_server(cache_style)
atom_cache_type cache_style;
{
    atom_server as = (atom_server) malloc(sizeof(atom_server_struct));

    nt_socket_init_func();
    if (atom_server_host == NULL) {	/* environment override */
	atom_server_host = cercs_getenv("ATOM_SERVER_HOST");
    }
    if (atom_server_host == NULL) {
	atom_server_host = ATOM_SERVER_HOST;	/* from configure */
    }
    as->server_id = atom_server_host;
    as->tcp_fd = -1;
    as->use_tcp = (cercs_getenv("ATL_USE_TCP") != NULL);

    Tcl_InitHashTable(&as->string_hash_table, TCL_STRING_KEYS);
    Tcl_InitHashTable(&as->value_hash_table, TCL_ONE_WORD_KEYS);
    as->hash_lock = thr_mutex_alloc();

#ifndef MODULE
    if ((as->he = gethostbyname(atom_server_host)) == NULL) {
	perror("gethostbyname (ATOM_SERVER_HOST)");
	exit(1);
    }
    if ((as->sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
	perror("socket");
	exit(1);
    }
#ifndef HAVE_WINDOWS_H
    as->flags = fcntl(as->sockfd, F_GETFL);
#else
    as->flags = 0;
#endif
    as->their_addr.sin_family = AF_INET;
    as->their_addr.sin_port = htons(UDP_PORT);
    as->their_addr.sin_addr = *((struct in_addr *) as->he->h_addr);
    memset(&(as->their_addr.sin_zero), '\0', 8);
#endif

    return as;
}
