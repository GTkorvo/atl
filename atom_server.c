#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#ifndef HAVE_WINDOWS_H
#include <unistd.h>
#include <netdb.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#include <signal.h>
#else 
#include <windows.h>
#include <winsock.h>
#endif
#include <memory.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <time.h>

#include <io.h>
#include <DE.h>
#include <atl.h>
#include <atom_formats.h>
#include <tclHash.h>
#include <unix_defs.h>

/* ### IT SHOULD BE FINE WITH CONFIG.H DEFINITIONS BUT ... */
/* Define to be the hostname to use as ATOM_SERVER_HOST */
#define ATOM_SERVER_HOST "marquesas.cc.gatech.edu"

/* Define to be the domain the atom server will serve */
#define ATOM_SERVICE_DOMAIN "gatech.edu"

extern int errno;
#ifndef PRINTF_DEFINED
extern int printf ARGS((const char *,...));
#endif
#ifndef FPRINTF_DEFINED
extern int fprintf ARGS((FILE *, const char *,...));
#endif
#ifndef STRDUP_DEFINED
extern char *strdup();
#endif
extern int atoi();
extern pid_t getpid();
#if !defined(GETHOSTNAME_DEFINED) & !defined(HAVE_WINDOWS_H)
extern int gethostname();
#endif
#if !defined(GETPEERNAME_DEFINED) & !defined(HAVE_WINDOWS_H)
extern int getpeername ARGS((int, struct sockaddr *, int *));
#endif
#if defined(HAVE_GETDOMAINNAME) && !defined(GETDOMAINNAME_DEFINED)
extern int getdomainname();
#endif
extern size_t strftime ARGS((char *String, size_t Length, const char *Format,
			     const struct tm *TmDate));
extern time_t   time ARGS((time_t *));

/* global variables */
DExchange de = NULL;
extern int de_debug_flag;

Tcl_HashTable string_hash_table;
Tcl_HashTable value_hash_table;

int send_get_format_id = 0;
int provisional_use_format_id = -1;
int next_atom_id = 1000;
char *server_id = NULL;

static int
handle_get_atom(de, dep, format_id, data, record_len, client_data)
DExchange de;
DEPort dep;
int format_id;
void *data;
int record_len;
void *client_data;
{
    send_get_atom_msg_ptr msg = (send_get_atom_msg_ptr) data;
    Tcl_HashEntry *entry = NULL;
    int error = 0;

    if ((msg->atom_string != NULL) && (msg->atom == 0)) {
	/* lookup atom translation from string */
	entry = Tcl_FindHashEntry(&string_hash_table, msg->atom_string);
	if (entry == NULL) {
	    int new;
	    char *str = strdup(msg->atom_string);
	    send_get_atom_msg_ptr stored =
	    (send_get_atom_msg_ptr) malloc(sizeof(send_get_atom_msg));

	    stored->atom_string = str;
	    stored->atom = next_atom_id++;

	    /* enter into string hash table */
	    entry = Tcl_CreateHashEntry(&string_hash_table, str, &new);
	    Tcl_SetHashValue(entry, stored);
	    if (!new) {
		fprintf(stderr, "Serious internal error in atom server.  Duplicate string hash entry.\n");
		exit(1);
	    }
	    /* enter into value hash table */
	    entry = Tcl_CreateHashEntry(&value_hash_table,
					(char *) stored->atom, &new);
	    Tcl_SetHashValue(entry, stored);
	    if (!new) {
		printf("Serious internal error in atom server.  Duplicate value hash entry.\n");
		exit(1);
	    }
	}
    } else if ((msg->atom_string == NULL) && (msg->atom != 0)) {
	/* lookup atom string from atom value */
	entry = Tcl_FindHashEntry(&value_hash_table, (char *) (msg->atom));
	if (entry == NULL) {
	    error = 1;
	}
    } else {
	/* what??? either both or neither specified... */
	error = 1;
    }
    if (error) {
	send_get_atom_msg error_msg;
	error_msg.atom_string = NULL;
	error_msg.atom = -1;

	DEport_write_data(dep, format_id, &error_msg);
	return 1;
    }
    msg = (send_get_atom_msg_ptr) Tcl_GetHashValue(entry);
    if (de_debug_flag) {
	printf(" Returning %d for %s\n", msg->atom, msg->atom_string);
    }
    DEport_write_data(dep, format_id, msg);
    return 1;
}

static int
handle_init_contact(de, dep, format_id, data, record_len, client_data)
DExchange de;
DEPort dep;
int format_id;
void *data;
int record_len;
void *client_data;
{
    init_contact_msg_ptr msg = (init_contact_msg_ptr) data;
    Tcl_HashEntry *entry;
    Tcl_HashSearch sptr;

    if (msg->send_id) {
	send_get_atom_msg send_msg;
	send_msg.atom_string = server_id;
	send_msg.atom = 0;
	DEport_write_data(dep, send_get_format_id, &send_msg);
    }
	
    if (msg->send_list) {
	send_get_atom_msg_ptr send_msg;
	send_get_atom_msg end_msg;

	end_msg.atom_string = NULL;
	end_msg.atom = 0;

	entry = Tcl_FirstHashEntry(&value_hash_table, &sptr);
	while (entry != NULL) {
	    send_msg = (send_get_atom_msg_ptr) Tcl_GetHashValue(entry);
	    if (de_debug_flag) {
		printf(" Dumping %d for %s\n", send_msg->atom,
		       send_msg->atom_string);
	    }
	    DEport_write_data(dep, send_get_format_id, send_msg);
	    entry = Tcl_NextHashEntry(&sptr);
	}
	DEport_write_data(dep, send_get_format_id, &end_msg);
    }
    return 1;
}

static char *atom_server_host = NULL;

static
int
connect_to_atom_server()
{
    DExchange de = DExchange_create();
    DEPort dep;

    if (atom_server_host == NULL) {	/* environment override */
	atom_server_host = getenv("ATOM_SERVER_HOST");
    }
    if (atom_server_host == NULL) {
	atom_server_host = ATOM_SERVER_HOST;	/* from configure */
    }

    dep = DExchange_initiate_conn(de, atom_server_host, PORT, 1);
    if (dep == NULL) {
	DExchange_close(de);
	return 0;
    }
    return 1;
}

static void
get_qual_hostname(char *buf, int len)
{
    struct hostent *host;

    char *network_string = getenv("DEXCHANGE_NETWORK");
    gethostname(buf, len);
    buf[len - 1] = '\0';
    if (memchr(buf, '.', strlen(buf)) == NULL) {
	/* no dots, probably not fully qualified */
	int end = strlen(buf);
	buf[end] = '.';
#ifdef HAVE_GETDOMAINNAME
	getdomainname((&buf[end]) + 1, len - strlen(buf));
	if (buf[end+1] == 0) {
	    char *tmp_name;
	    buf[end] = 0;
	    /* getdomainname was useless, hope that gethostbyname helps */
	    tmp_name = (gethostbyname(buf))->h_name;
	    strncpy(buf, tmp_name, len);
	}
#else
	/* no getdomainname, hope that gethostbyname will help */
	char *tmp_name = (gethostbyname(buf))->h_name;
	strncpy(buf, tmp_name, len);
#endif
	buf[len - 1] = '\0';
    }
    if (((host = gethostbyname(buf)) == NULL) ||
	(memchr(buf, '.', strlen(buf)) == NULL)){
	/* bloody hell, what do you have to do? */

	gethostname(buf, len);
	host  = gethostbyname(buf);
	host = gethostbyaddr(host->h_addr_list[0], host->h_length, host->h_addrtype);
	if (host != NULL) {
	  strncpy(buf, host->h_name, len);
	}
    }
    if (((host = gethostbyname(buf)) == NULL) ||
	(memchr(buf, '.', strlen(buf)) == NULL)){
	static int warn_once = 0;
	if (warn_once == 0) {
	    warn_once++;
	    printf("Attempts to establish your fully qualified hostname have failed horribly.  Sorry.\n");
	}
    }
}

typedef struct host_info {
    struct in_addr ip_addr;
    time_t intro_time;
} *host_info_p;

static host_info_p hostlist = NULL;
static int host_count = 0;

static time_t
get_time_for_host(ip_addr, hostname)
struct in_addr ip_addr;
char *hostname;
{
    int i;
    if (hostlist == NULL) {
	hostlist = malloc(sizeof(struct host_info));
    }
    for (i = 0; i< host_count; i++) {
	if (memcmp(&ip_addr, &hostlist[i].ip_addr, sizeof(struct in_addr)) == 0) {
	    return hostlist[i].intro_time;
	}
    }
    hostlist = realloc(hostlist, sizeof(struct host_info) * (host_count + 1));
    memcpy(&hostlist[host_count].ip_addr, &ip_addr, sizeof(struct in_addr));
    hostlist[host_count].intro_time = time(NULL);
    host_count++;
    return hostlist[host_count-1].intro_time;
}

#define GRACE_PERIOD_SEC 60 * 60 * 24 * 3

static char *postfix = ATOM_SERVICE_DOMAIN;

static void
out_domain_rejection(de, dep)
DExchange de;
DEPort dep;
{
    struct sockaddr sock_addr;
    int sock_len = sizeof(sock_addr);
    IOFile from_port = DEport_from_port(dep);
    time_t intro_time;

    if (from_port == NULL) {
	DEport_close(dep);
	return;
    }
    if (getpeername(file_id_IOfile(from_port), &sock_addr,
		    &sock_len) == 0) {
	struct hostent *host;
	if (sock_addr.sa_family == AF_INET) {
	    struct sockaddr_in *in_sock = (struct sockaddr_in *) &sock_addr;
	    host = gethostbyaddr((char *) &in_sock->sin_addr,
				 sizeof(struct in_addr), AF_INET);
	    if (host != NULL) {
		char **alias;
		int postlen = strlen(postfix);
		int hostlen = strlen(host->h_name);
		/* 
		 * Check to see if our ATOM_SERVICE_DOMAIN is a postfix
		 * of the hostname.  If not, we'll reject the host as being
		 * outside of our service domain.
		 */
		if (hostlen > postlen) {
		    if (strcmp(&host->h_name[hostlen - postlen], postfix) == 0) {
			/* good host */
			return;
		    }
		} else {
		    if (strcmp(host->h_name, "localhost") == 0) {
			return;
		    }
		}
		for (alias = host->h_aliases; *alias != 0; alias++) {
		    if (hostlen > postlen) {
			if (strcmp(alias[hostlen - postlen], postfix) == 0) {
			    /* good host */
			    return;
			}
		    }
		}

		intro_time = get_time_for_host(in_sock->sin_addr, host->h_name);
		if ((time(NULL) - intro_time) < GRACE_PERIOD_SEC) {
		    provisional_use_msg msg;
		    msg.domain = postfix;
		    msg.time = intro_time + GRACE_PERIOD_SEC - time(NULL);
		    DEport_write_data(dep, provisional_use_format_id, &msg);
		    return;
		}

		printf("Rejecting out-domain host %s, aliases: ", host->h_name);
		for (alias = host->h_aliases; *alias != 0; alias++) {
		    printf(" %s", *alias);
		}
		printf("\n");
	    }
	}
    }
    /* must not be a good host, close it down ... */
    DEport_close(dep);
}

static void 
die_with_error(sig)
int sig;
{
    fprintf(stderr, "Atom server not responding.  Timeout.\n");
    exit(1);
}

extern int
main(argc, argv)
int argc;
char **argv;
{
    int i, no_fork = 0;
    char stack_server_id[1024];
    char host_name[256];
    char time_str[256];
    char env[128];
    time_t current_time;

    de = DExchange_create();
    for (i = 1; i < argc; i++) {
	if (!strcmp(argv[i], "-debug")) {
	    de_debug_flag = 1;
	} else if (strcmp(argv[i], "-no_fork") == 0) {
	    no_fork++;
	} else {
	    printf("Unknown flag %s\n", argv[i]);
	    exit(1);
	}
    }

    /* substitute this host name for the atom server host */
    strcpy(env, "ATOM_SERVER_HOST=");
    gethostname(&env[strlen(env)], sizeof(env) - strlen(env));
    putenv(env);

#ifndef HAVE_WINDOWS_H
    signal(SIGALRM, die_with_error);
    alarm(30);
#endif
    if (connect_to_atom_server()) {
	/* already running */
	exit(0);
#ifndef HAVE_WINDOWS_H
    } else {
	/* make really, really certain there's no atom_server running */
	FILE *atom_server_pid_file = fopen("/tmp/atom_server_pid", "r");
	alarm(0);
	if (atom_server_pid_file != NULL) {
	    long server_pid;
	    if (fscanf(atom_server_pid_file, "%lx", &server_pid) == 1) {
		if (kill(server_pid, 0) == 0 ) {
		    fprintf(stderr, "Atom Server %lx not responding, but still running\n",
			   server_pid);
		    exit(0);
		}
	    }
	    fclose(atom_server_pid_file);
	}

	if (!no_fork) {
	    if (fork() != 0) {
		/* I'm the parent, return now */
		exit(0);
	    }
	}
	atom_server_pid_file = fopen("/tmp/atom_server_pid", "w");
	if (atom_server_pid_file) {
	    fprintf(atom_server_pid_file, "%lx\n", (long)getpid());
	    fclose(atom_server_pid_file);
	}
	if (!no_fork) {
	    if (fork() != 0) {
	        /* I'm the parent, return now */
		exit(0);
	    }
	}
#endif
    }

    if (DExchange_listen(de, PORT) != 0) {
	fprintf(stderr, "DExchangee listen failed\n");
	exit(1);
    }
    DExchange_set_forward(de, DENever_Forward);
    DExchange_register_close_handler(de, (OpenCloseHandlingFunc) 0);

    DExchange_register_format(de, "get atom", Atom_send_get_msg_flds);
    DExchange_register_function(de, "get atom", handle_get_atom, NULL);
    send_get_format_id = DEget_format_id(de, "get atom");

    DExchange_register_format(de, "init contact", Atom_init_contact_msg_flds);
    DExchange_register_function(de, "init contact", handle_init_contact, NULL);

    DExchange_register_format(de, "provisional use",
			      atom_provisional_use_msg_flds);
    provisional_use_format_id = DEget_format_id(de, "provisional use");

    if (strcmp(ATOM_SERVICE_DOMAIN, "") != 0) {
	DExchange_register_open_handler(de, out_domain_rejection);
    }
    Tcl_InitHashTable(&string_hash_table, TCL_STRING_KEYS);
    Tcl_InitHashTable(&value_hash_table, TCL_ONE_WORD_KEYS);

    get_qual_hostname(host_name, sizeof(host_name));
    time(&current_time);
    strftime(time_str, sizeof(time_str), "%m/%d/%Y %R", localtime(&current_time));
    sprintf(stack_server_id, "Host: %s, Port %d, PID %lx, Start: %s",
	    host_name, PORT, (long)getpid(), time_str);
    server_id = strdup(stack_server_id);
    while (1) {
	DExchange_poll_and_handle(de, 1);
    }
}
