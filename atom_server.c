#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <tcl.h>
typedef int atom_t;
#include "atom_internal.h"

#define MAXBUFLEN 100

extern Tcl_HashTable *stringhash;
extern Tcl_HashTable *valuehash;
extern char *atom_to_string(Tcl_HashTable *, Tcl_HashTable *, int);
extern int string_to_atom(Tcl_HashTable *, Tcl_HashTable *, char *);
extern int set_string_and_atom(Tcl_HashTable *, Tcl_HashTable *, char *, int);


Tcl_HashTable *stringhash;
Tcl_HashTable *valuehash;

int verbose = 0;

char *
atom_to_string(Tcl_HashTable * string_hash_table, Tcl_HashTable * value_hash_table, int value)
{
    Tcl_HashEntry *entry = NULL;
    send_get_atom_msg_ptr value_string;

    if (verbose) printf("Doing a atom_to_string\n");

    entry = Tcl_FindHashEntry(value_hash_table, (char *) value);

    if (entry) {
	value_string = (send_get_atom_msg_ptr) Tcl_GetHashValue(entry);
	return value_string->atom_string;
    }
    return 0;

}

int
string_to_atom(Tcl_HashTable * string_hash_table, Tcl_HashTable * value_hash_table, char *a)
{
    static int next_atom_value = 2000;

    Tcl_HashEntry *entry = NULL;
    send_get_atom_msg_ptr return_msg;

    if (verbose) printf("Doing a string_to_atom\n");

    entry = Tcl_FindHashEntry(string_hash_table, a);

    if (entry) {
	return_msg = (send_get_atom_msg_ptr) Tcl_GetHashValue(entry);
	return return_msg->atom;
    } else {
	set_string_and_atom(string_hash_table, value_hash_table, a,
			    next_atom_value++);
    }
    return next_atom_value - 1;	/* the string was not in the db */

}

int
set_string_and_atom(Tcl_HashTable * string_hash_table, Tcl_HashTable * value_hash_table, char *a, int set_atom)
{

    send_get_atom_msg_ptr return_msg;
    Tcl_HashEntry *entry = NULL;
    int new;
    send_get_atom_msg_ptr stored;

    stored = (send_get_atom_msg_ptr) malloc(sizeof(send_get_atom_msg));
    stored->atom_string = strdup(a);

    stored->atom = set_atom;
    entry = Tcl_CreateHashEntry(string_hash_table, a, &new);
    Tcl_SetHashValue(entry, stored);
    entry = Tcl_CreateHashEntry(value_hash_table, (char *) stored->atom, &new);
    Tcl_SetHashValue(entry, stored);
    return_msg = (send_get_atom_msg_ptr) Tcl_GetHashValue(entry);
    return return_msg->atom;	/* Success */

}

void
Initialize(void)
{
    stringhash = (Tcl_HashTable *) malloc(sizeof(Tcl_HashTable));
    Tcl_InitHashTable(stringhash, TCL_STRING_KEYS);
    valuehash = (Tcl_HashTable *) malloc(sizeof(Tcl_HashTable));
    Tcl_InitHashTable(valuehash, TCL_ONE_WORD_KEYS);
}

int
main(void)
{
    int sockfd;
    struct sockaddr_in my_addr;	/* my address information */

    struct sockaddr_in their_addr;	/* connector's address information 
					 */

    int addr_len, numbytes;
    char buf[MAXBUFLEN];

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
	perror("socket");
	exit(1);
    }
    my_addr.sin_family = AF_INET;	/* host byte order */

    my_addr.sin_port = htons(PORT);	/* short, network byte order */

    my_addr.sin_addr.s_addr = INADDR_ANY;	/* automatically fill with 
						 * my IP */

    memset(&(my_addr.sin_zero), '\0', 8);	/* zero the rest of the
						 * struct */

    if (bind(sockfd, (struct sockaddr *) &my_addr,
	     sizeof(struct sockaddr)) == -1) {
	perror("bind");
	exit(1);
    }
    addr_len = sizeof(struct sockaddr);

    Initialize();

    while (1) {

	if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN - 1, 0,
		    (struct sockaddr *) &their_addr, &addr_len)) == -1) {
	    perror("recvfrom");
	    exit(1);
	}
	buf[numbytes] = 0;

	if (verbose) printf("recd. %s\n", buf);

	switch(buf[0]) {
	case 'N': {
	    char *str;
	    char buf2[MAXBUFLEN];
	    int atom = 0;
	    if (sscanf(&buf[1], "%d", &atom) != 1) break;
	    str = atom_to_string(stringhash, valuehash, atom);
	    buf2[0] = 'S';
	    if (str) {
		strncpy(&buf2[1], str, sizeof(buf2));
	    } else {
		buf2[1] = 0;
	    }
	    if (verbose) printf("Sending %s\n", buf2);

	    if ((numbytes = sendto(sockfd, buf2, strlen(buf2), 0,
				   (struct sockaddr *) &their_addr, sizeof(struct sockaddr))) == -1) {
		perror("sendto");
		exit(1);
	    }
	    break;
	}
	case 'S': {
	    /* get value for string */
	    char num_str[12];
	    int value = string_to_atom(stringhash, valuehash, &buf[1]);

	    sprintf(num_str, "N%d", value);
	    if (verbose) printf("Sending %s\n", num_str);
	    
	    if ((numbytes = sendto(sockfd, num_str, strlen(num_str), 0,
				   (struct sockaddr *) &their_addr, 
				   sizeof(struct sockaddr))) == -1) {
		perror("sendto");
		exit(1);
	    }
	    break;
	}
	case 'A': {
	    int atom = 0;
	    char buf2[MAXBUFLEN];
	    char *str;
	    Tcl_HashEntry *entry = NULL;

	    atom = strtol(&buf[1], &str, 10);
	    str++;
	    entry = Tcl_FindHashEntry(stringhash, str);
	    if (entry != NULL) {
		send_get_atom_msg_ptr atom_entry = 
		    (send_get_atom_msg_ptr) Tcl_GetHashValue(entry);
		if ((atom_entry != NULL) && (atom_entry->atom != atom)) {
		    if (verbose) printf("Atom cache inconsistency, tried to associate string \"%s\" with value %d\n	Previous association was value %d\n",
			   str, atom, atom_entry->atom);
		    sprintf(buf2, "E%d %s", atom_entry->atom,
			    atom_entry->atom_string);
		    if (verbose) printf("Sending %s\n", buf2);
		    
		    if ((numbytes = sendto(sockfd, buf2, strlen(buf2), 0,
					   (struct sockaddr *) &their_addr, 
					   sizeof(struct sockaddr))) == -1) {
			perror("sendto");
			exit(1);
		    }
		}
	    }
	    entry = Tcl_FindHashEntry(valuehash, (char *) atom);
	    if (entry != NULL) {
		send_get_atom_msg_ptr atom_entry = 
		    (send_get_atom_msg_ptr) Tcl_GetHashValue(entry);
		if ((atom_entry != NULL) && 
		    (strcmp(atom_entry->atom_string, str) != 0)) {
		    if (verbose) printf("Atom cache inconsistency, tried to associate value %d with string \"%s\"\n	Previous association was string \"%s\"\n",
			   atom, str, atom_entry->atom_string);
		    sprintf(buf2, "E%d %s", atom_entry->atom,
			    atom_entry->atom_string);
		    if (verbose) printf("Sending %s\n", buf2);
		    
		    if ((numbytes = sendto(sockfd, buf2, strlen(buf2), 0,
					   (struct sockaddr *) &their_addr, 
					   sizeof(struct sockaddr))) == -1) {
			perror("sendto");
			exit(1);
		    }
		}
	    }
	    set_string_and_atom(stringhash, valuehash, str, atom);
	    break;
	}
	}
    }
    /* NOTREACHED */

    close(sockfd);
    return 0;
}
