#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#ifndef HAVE_WINDOWS_H
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#else
#include <windows.h>
#include <winsock.h>
#define EWOULDBLOCK WSAEWOULDBLOCK
#endif
#include <fcntl.h>
#include <tclHash.h>
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

int verbose = 1;

char *
atom_to_string(Tcl_HashTable * string_hash_table, Tcl_HashTable * value_hash_table, int value)
{
    Tcl_HashEntry *entry = NULL;
    send_get_atom_msg_ptr value_string;

    if (verbose)
	printf("Doing a atom_to_string\n");

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

    if (verbose)
	printf("Doing a string_to_atom\n");

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

#ifndef O_NONBLOCK
#define O_NONBLOCK 0x80
#endif
static void
set_blocking(fd, block)
int fd;
int block;
{
    int flags = 0;
    if (block) {
	flags &= (~O_NONBLOCK);
    } else {
	flags |= O_NONBLOCK;
    }
#ifndef HAVE_WINDOWS_H
    if (fcntl(fd, F_SETFL, flags) < 0) {
	perror("fcntl");
	exit(1);
    }
#else
    if (ioctlsocket(fd, FIONBIO, (unsigned long*)!block) != 0) {
	perror("ioctlsocket");
	exit(1);
    }
#endif
}

int
main(argc, argv)
int argc;
char **argv;
{
    int sockfd;
    int quiet = 0;
    int run = 1;
    int no_fork = 0;
    struct sockaddr_in my_addr;	/* my address information */
    struct sockaddr_in their_addr;	/* connector's address information */

    int addr_len, numbytes, i;
    char buf[MAXBUFLEN];

    for (i = 1; i < argc; i++) {
	if (strcmp(argv[i], "-no_fork") == 0) {
	    no_fork++;
	} else if (strcmp(argv[i], "-quiet") == 0) {
	    quiet = 1;
	    verbose = 0;
	} else if (strcmp(argv[i], "-verbose") == 0) {
	    quiet = 0;
	    verbose = 1;
	} else {
	    fprintf(stderr, "Unknown argument \"%s\"\n", argv[i]);
	    exit(1);
	}
    }

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
	perror("socket");
	exit(1);
    }
    my_addr.sin_family = AF_INET;	/* host byte order */
    my_addr.sin_port = htons(PORT);	/* short, network byte order */
    my_addr.sin_addr.s_addr = INADDR_ANY; /* automatically fill with my IP */
    memset(&(my_addr.sin_zero), '\0', 8);	/* zero the rest of the *
						 * struct */
    {
	char ping_char = 'P';
	int start_time;
	int cur_time;
	int test_fd;
	char response_char = 0;
	if (!quiet)
	    printf("Checking for already running atom server.\nPlease wait.");
	fflush(stdout);
	if ((test_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
	    perror("socket");
	    exit(1);
	}
	set_blocking(test_fd, 0);
	cur_time = start_time = time(NULL);
	while ((response_char != 'R') && ((cur_time - start_time < 30))) {
	    if ((numbytes = sendto(test_fd, &ping_char, 1, 0,
				   (struct sockaddr *) &my_addr,
				   sizeof(struct sockaddr))) == -1) {
		perror("sendto");
		exit(1);
	    }
	    sleep(1);
	    if ((numbytes = recvfrom(test_fd, &response_char, 1, 0,
				     NULL, NULL)) == -1) {
		if (errno != EWOULDBLOCK) {
		    perror("recvfrom");
		    exit(1);
		}
	    }
	    if (!quiet)
		printf(".");
	    fflush(stdout);
	    cur_time = time(NULL);
	}
	if (response_char == 'R') {
	    if (!quiet)
		printf("\n\tAtom server already running\n");
	    exit(0);
	}
	close(test_fd);
    }
    if (!quiet)
	printf("\n\tNo Atom server found - ");
#ifdef HAVE_FORK
    if (!no_fork) {
	if (!quiet)
	    printf("Forking server to background\n");
	if (fork() != 0) {
	    /* I'm the parent, return now */
	    exit(0);
	}
    } else {
	if (!quiet)
	    printf("Running...\n");
    }
#endif

    if (bind(sockfd, (struct sockaddr *) &my_addr,
	     sizeof(struct sockaddr)) == -1) {
	perror("bind");
	exit(1);
    }
    addr_len = sizeof(struct sockaddr);

    run = 1;
    Initialize();

    while (run) {
	if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN - 1, 0,
		    (struct sockaddr *) &their_addr, &addr_len)) == -1) {
	    perror("recvfrom");
	    exit(1);
	}
	buf[numbytes] = 0;

	if (verbose)
	    printf("recd. %s\n", buf);

	switch (buf[0]) {
	case 'P':{
		/* Ping */
		char response_char = 'R';
		if (verbose)
		    printf("Sending %c\n", response_char);
		if ((numbytes = sendto(sockfd, &response_char, 1, 0,
				       (struct sockaddr *) &their_addr,
				       sizeof(struct sockaddr))) == -1) {
		    perror("sendto");
		    exit(1);
		}
		break;
	    }
	case 'N':{
		/* request translation of numeric value to a string */
		char *str;
		char buf2[MAXBUFLEN];
		int atom = 0;
		if (sscanf(&buf[1], "%d", &atom) != 1)
		    break;
		str = atom_to_string(stringhash, valuehash, atom);
		buf2[0] = 'S';
		if (str) {
		    strncpy(&buf2[1], str, sizeof(buf2));
		} else {
		    buf2[1] = 0;
		}
		if (verbose)
		    printf("Sending %s\n", buf2);

		if ((numbytes = sendto(sockfd, buf2, strlen(buf2), 0,
				       (struct sockaddr *) &their_addr, sizeof(struct sockaddr))) == -1) {
		    perror("sendto");
		    exit(1);
		}
		break;
	    }
	case 'S':{
		/* request translation of string to a numeric value */
		char num_str[12];
		int value = string_to_atom(stringhash, valuehash, &buf[1]);

		sprintf(num_str, "N%d", value);
		if (verbose)
		    printf("Sending %s\n", num_str);

		if ((numbytes = sendto(sockfd, num_str, strlen(num_str), 0,
				       (struct sockaddr *) &their_addr,
				       sizeof(struct sockaddr))) == -1) {
		    perror("sendto");
		    exit(1);
		}
		break;
	    }
	case 'A':{
		/* create an association between a string and a value */
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
			if (verbose)
			    printf("Atom cache inconsistency, tried to associate string \"%s\" with value %d\n	Previous association was value %d\n",
				   str, atom, atom_entry->atom);
			sprintf(buf2, "E%d %s", atom_entry->atom,
				atom_entry->atom_string);
			if (verbose)
			    printf("Sending %s\n", buf2);

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
			if (verbose)
			    printf("Atom cache inconsistency, tried to associate value %d with string \"%s\"\n	Previous association was string \"%s\"\n",
				   atom, str, atom_entry->atom_string);
			sprintf(buf2, "E%d %s", atom_entry->atom,
				atom_entry->atom_string);
			if (verbose)
			    printf("Sending %s\n", buf2);

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
