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

#include "unix_defs.h"
#include <gen_thread.h>
#else

#include "kernel/katl.h"
#include "kernel/library.h"
#include "kernel/kernel_defs.h"
#include "assert.h"
#endif

#define MAXDATASIZE 100
#include "atom_formats.h"
//#include "tclHash.h"

/* ### IT SHOULD BE FINE WITH CONFIG.H DEFINITIONS BUT ... */
/* Define to be the hostname to use as ATOM_SERVER_HOST */
#define ATOM_SERVER_HOST "blackspruce.cc.gatech.edu"

/* Define to be the domain the atom server will serve */
#define ATOM_SERVICE_DOMAIN "gatech.edu"

/* opaque type for atom server handle */
typedef struct _atom_server {
#ifdef USE_DATAEXCHANGE
    DExchange de;
    DEPort dep;
#endif
#ifdef USE_UDP
    int sockfd, addr_len;
    struct hostent *he;
    struct sockaddr_in their_addr;
#endif  
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

int string_to_int(char *str){
 int no = 0, i = 0;

 do{
  no *= 10;
  no += str[i]-'0';
  i++;
 }while(str[i]<='9' && str[i]>='0');
 return no;
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
    Tcl_SetHashValue(entry, msg);
    /* enter into value hash table */
    entry = Tcl_CreateHashEntry(&as->value_hash_table,
				 stored->atom, &new);
    if (!new) {
	printf("Serious internal error in atom cache.  Duplicate value hash entry.\n");
	thr_mutex_unlock(as->hash_lock);
	exit(1);
    }
    Tcl_SetHashValue(entry, msg);
    thr_mutex_unlock(as->hash_lock);
    return stored;
}

#ifdef USE_DATAEXCHANGE
static void
handle_prov_msg(as, msg)
atom_server as;
provisional_use_msg_ptr msg;
{
    static int warned = 0;

    if (warned) return;

    warned++;
    fprintf(stderr, "The atom_server daemon on %s serves the domain \"%s\"\n",
	    DEport_host_name(as->dep), msg->domain);
    fprintf(stderr, " See http://www.cc.gatech.edu/systems/projects/MOSS/servers.html for more info.\n");
    fprintf(stderr, "  Temporary use allowed for ");
    if (msg->time > 60*60*48) {  /* 2 days */
	int days = msg->time / (60 * 60 * 24);
	fprintf(stderr, "%d more days.\n", days);
    } else if (msg->time > 60*60*2) {
	int hours = msg->time / (60 * 60 );
	fprintf(stderr, "%d more hours.\n", hours);
    } else {
	int mins = msg->time / 60 ;
	fprintf(stderr, "%d more minutes.\n", mins);
    }
}	
#endif

extern
 atom_t
set_string_and_atom(as, str, atom)
atom_server as;
char *str;
atom_t atom;
{
    send_get_atom_msg_ptr return_msg;
    Tcl_HashEntry *entry = NULL, *entry2 = NULL;
            int new;
            send_get_atom_msg_ptr stored =
            (send_get_atom_msg_ptr) malloc(sizeof(send_get_atom_msg));

#ifdef USE_UDP
    int numbytes,len;
    char buf[MAXDATASIZE];
    int addr_len = sizeof(struct sockaddr);
#endif

//printf("Entered set_string_and_atom\n");
    thr_mutex_lock(as->hash_lock);
    entry = Tcl_FindHashEntry(&as->string_hash_table, str);
    entry2 = Tcl_FindHashEntry(&as->value_hash_table, atom);
    thr_mutex_unlock(as->hash_lock);
    if (entry == NULL && entry2 == NULL) {

            stored->atom = atom;
            stored->atom_string = strdup(str);
            sprintf(buf,"%s %d",str,atom);
            len = strlen(buf);
#ifdef USE_UDP
       if((numbytes=sendto(as->sockfd, buf, len, 0,
                     (struct sockaddr *)&(as->their_addr), sizeof(struct sockaddr))) == -1) {
        perror("sendto");
        exit(1);
       }
       if((numbytes=recvfrom(as->sockfd, buf, MAXDATASIZE-1 , 0,
                (struct sockaddr *)&(as->their_addr), &addr_len)) == -1) {
        perror("recvfrom");
        exit(1);
        }
       buf[numbytes] = 0;
  if(!string_to_int(buf)) return 0; 

#endif

            stored->atom = string_to_int(buf);
            thr_mutex_lock(as->hash_lock);
            /* enter into string hash table */
            entry = Tcl_CreateHashEntry(&as->string_hash_table, str, &new);
            Tcl_SetHashValue(entry, stored);
            if (!new) {
                fprintf(stderr, "2. Serious internal error in atom server.  Duplicate string hash entry.\n");
                exit(1);
            }
            /* enter into value hash table */
            entry = Tcl_CreateHashEntry(&as->value_hash_table,
                                        stored->atom , &new);
            Tcl_SetHashValue(entry, stored);
            if (!new) {
                printf("3. Serious internal error in atom server.  Duplicate value hash entry.\n");
                exit(1);
            }
            thr_mutex_unlock(as->hash_lock);

        if (as->cache_style != no_atom_cache) {
            return_msg = enter_atom_into_cache(as, stored);
        }
        return stored->atom;
    } else {

 if(entry)   return_msg = (send_get_atom_msg_ptr) Tcl_GetHashValue(entry);
 else  return_msg = (send_get_atom_msg_ptr) Tcl_GetHashValue(entry2);
//printf("Already there in cache.. %s %d\n", return_msg->atom_string, return_msg->atom);
        return return_msg->atom;
    }

} 

extern
 atom_t
atom_from_string(as, str)
atom_server as;
char *str;
{
    send_get_atom_msg inquiry_msg;
    send_get_atom_msg_ptr return_msg;
    Tcl_HashEntry *entry = NULL;
#ifdef USE_UDP
    int numbytes,len;
    char buf[MAXDATASIZE];
#endif 
     int new;
            send_get_atom_msg_ptr msg;
            send_get_atom_msg_ptr stored =
            (send_get_atom_msg_ptr) malloc(sizeof(send_get_atom_msg));

    inquiry_msg.atom_string = str;
    inquiry_msg.atom = 0;
     msg = &inquiry_msg;

    if (gen_thr_initialized()) {
	if (as->hash_lock == NULL) {
	    as->hash_lock = thr_mutex_alloc();
	}
	thr_mutex_lock(as->hash_lock);
    }
    entry = Tcl_FindHashEntry(&as->string_hash_table, str);
    thr_mutex_unlock(as->hash_lock);
    if (entry == NULL) {
#ifndef USE_DATAEXCHANGE
#ifdef USE_UDP

    len = strlen(str);
//fprintf(stderr,"ATLATLATLATLSending %s\n", str);
    if(sendto(as->sockfd, str, len, 0, (struct sockaddr *)&(as->their_addr), sizeof(struct sockaddr)) == -1) perror("sendto");
    if ((numbytes=recvfrom(as->sockfd, buf, MAXDATASIZE-1, 0, (struct sockaddr *)&(as->their_addr), &(as->addr_len))) == -1) {
        perror("recvfrom");
        exit(1);
       }
       buf[numbytes] = 0;
/*
    return_msg = (send_get_atom_msg_ptr)malloc(sizeof(send_get_atom_msg));
    return_msg->atom_string = (char *)malloc(sizeof(char)*len);
    strcpy(return_msg->atom_string, str);
    return_msg->atom = string_to_int(buf);
*/
#endif  

	    stored->atom_string = str;
	    stored->atom = as->next_atom_id++;

	    thr_mutex_lock(as->hash_lock);
	    /* enter into string hash table */
	    entry = Tcl_CreateHashEntry(&as->string_hash_table, str, &new);
	    Tcl_SetHashValue(entry, stored);
	    if (!new) {
		fprintf(stderr, "Serious internal error in atom server.  Duplicate string hash entry.\n");
		exit(1);
	    }
	    /* enter into value hash table */
	    entry = Tcl_CreateHashEntry(&as->value_hash_table,
					(char *) stored->atom, &new);
	    Tcl_SetHashValue(entry, stored);
	    if (!new) {
		printf("Serious internal error in atom server.  Duplicate value hash entry.\n");
		exit(1);
	    }
	    return_msg =  (send_get_atom_msg_ptr) Tcl_GetHashValue(entry);
	    thr_mutex_unlock(as->hash_lock);
#else
	int return_format_id;
	DEport_write_data(as->dep, as->get_send_format_id,
			   &inquiry_msg);

	return_msg =
	    (send_get_atom_msg_ptr) DEport_read_data(as->dep,
						     &return_format_id);

	if (return_format_id == as->prov_use_format_id) {
	    handle_prov_msg(as, (provisional_use_msg_ptr)return_msg);
	    return_msg = (send_get_atom_msg_ptr) 
		DEport_read_data(as->dep, &return_format_id);
	}

	assert(return_format_id == as->get_send_format_id);
#endif
	if (as->cache_style != no_atom_cache) {
	    return_msg = enter_atom_into_cache(as, return_msg);
	}
    } else {
	return_msg = (send_get_atom_msg_ptr) Tcl_GetHashValue(entry);
    }
    return return_msg->atom;
}

extern
char *
string_from_atom(as, atom)
atom_server as;
atom_t atom;
{
    send_get_atom_msg inquiry_msg;
    send_get_atom_msg_ptr return_msg;
    Tcl_HashEntry *entry = NULL;
#ifdef USE_UDP
    int numbytes;
    char buf[MAXDATASIZE];
#endif

    inquiry_msg.atom_string = NULL;
    inquiry_msg.atom = atom;

    if (gen_thr_initialized()) {
	if (as->hash_lock == NULL) {
	    as->hash_lock = thr_mutex_alloc();
	}
	thr_mutex_lock(as->hash_lock);
    }
    entry = Tcl_FindHashEntry(&as->value_hash_table, atom);
    thr_mutex_unlock(as->hash_lock);

    if (entry == NULL) {
#ifndef USE_DATAEXCHANGE
//	assert(0);
//	return NULL;
#ifdef USE_UDP

    sprintf(buf, "%d",atom);
//fprintf(stderr,"ATLATLATLATLSending %s\n", buf);
    if(sendto(as->sockfd, buf, strlen(buf), 0, (struct sockaddr *)&(as->their_addr), sizeof(struct sockaddr)) == -1) perror("send");
    if ((numbytes=recvfrom(as->sockfd, buf, MAXDATASIZE-1, 0, (struct sockaddr *)&(as->their_addr), &(as->addr_len))) == -1) {
        perror("recv");
        exit(1);
       }
       buf[numbytes] = 0;

    if(buf[0] == '0' && buf[1] == 0) return NULL;

    return_msg = (send_get_atom_msg_ptr)malloc(sizeof(send_get_atom_msg));
    return_msg->atom_string = strdup(buf);
    return_msg->atom = atom;

        if (as->cache_style != no_atom_cache) {
            enter_atom_into_cache(as, return_msg);
        }
#endif  
#else
	int return_format_id;
	DEport_write_data(as->dep, as->get_send_format_id,
			   &inquiry_msg);

	return_msg = (send_get_atom_msg_ptr)
	    DEport_read_data(as->dep, &return_format_id);

	if (return_format_id == as->prov_use_format_id) {
	    handle_prov_msg(as, (provisional_use_msg_ptr)return_msg);
	    return_msg = (send_get_atom_msg_ptr) 
		DEport_read_data(as->dep, &return_format_id);
	}

	assert(return_format_id == as->get_send_format_id);

	if (as->cache_style != no_atom_cache) {
	    return_msg = enter_atom_into_cache(as, return_msg);
	}
#endif
    } else {
	return_msg = (send_get_atom_msg_ptr) Tcl_GetHashValue(entry);
    }
    if (return_msg->atom_string != NULL) {
	return strdup(return_msg->atom_string);
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
    as->server_id = "self";
#ifdef USE_DATAEXCHANGE
    as->de = DExchange_create();
    as->dep = DExchange_initiate_conn(as->de, atom_server_host,
				    PORT, 1);
    if (as->dep == NULL) {
	fprintf(stderr, "Failed to connect to atom server on host %s.  Is it running?\n",
		atom_server_host);
	exit(1);
    }

    DExchange_register_format(as->de, "get atom", Atom_send_get_msg_flds);
    DExchange_set_format_fixed(as->de, "get atom", 1);

    DExchange_register_format(as->de, "init contact", Atom_init_contact_msg_flds);
    DExchange_set_format_fixed(as->de, "init contact", 1);

    as->get_send_format_id = DEget_format_id(as->de, "get atom");
    DExchange_register_format(as->de, "provisional use",
			      atom_provisional_use_msg_flds);
    as->prov_use_format_id = DEget_format_id(as->de, "provisional use");

    DEport_set_format_block(as->dep, "provisional use", FALSE);
#endif

    Tcl_InitHashTable(&as->string_hash_table, TCL_STRING_KEYS);
    Tcl_InitHashTable(&as->value_hash_table, TCL_ONE_WORD_KEYS);
    as->hash_lock = thr_mutex_alloc();
    as->next_atom_id = 1000;
    as->cache_style = cache_style;
#ifdef USE_DATAEXCHANGE
    {
	send_get_atom_msg_ptr return_msg;
	int return_format_id;
	init_contact_msg init_msg;

	init_msg.send_id = 1;
	if (cache_style != prefill_atom_cache) {
	    init_msg.send_list = 0;
	    DEport_write_data_by_name(as->dep, "init contact", &init_msg);

	    /* receive server ID */
	    return_msg = (send_get_atom_msg_ptr)
		DEport_read_data(as->dep, &return_format_id);
	    if (return_format_id == as->prov_use_format_id) {
		handle_prov_msg(as, (provisional_use_msg_ptr)return_msg);
		return_msg = (send_get_atom_msg_ptr) 
		    DEport_read_data(as->dep, &return_format_id);
	    }
	    
	    as->server_id = strdup(return_msg->atom_string);
	} else {
	    
	    init_msg.send_list = 1;
	    DEport_write_data_by_name(as->dep, "init contact", &init_msg);
	    
	    /* receive server ID */
	    return_msg = (send_get_atom_msg_ptr)
		DEport_read_data(as->dep, &return_format_id);
	    if (return_format_id == as->prov_use_format_id) {
		handle_prov_msg(as, (provisional_use_msg_ptr)return_msg);
		return_msg = (send_get_atom_msg_ptr) 
		    DEport_read_data(as->dep, &return_format_id);
	    }
	    
	    as->server_id = strdup(return_msg->atom_string);
	    
	    /* receive atom list, sequence terminated with 0 atom value */
	    return_msg = (send_get_atom_msg_ptr)
		DEport_read_data(as->dep, &return_format_id);
	    while (return_msg->atom != 0) {
		(void) enter_atom_into_cache(as, return_msg);
		return_msg = (send_get_atom_msg_ptr)
		    DEport_read_data(as->dep, &return_format_id);
	    }
	}
    }
#endif
#ifdef USE_UDP

 if ((as->he=gethostbyname(ATOM_SERVER_HOST)) == NULL) {  // get the host info
        perror("gethostbyname");
        exit(1);
    }

    if ((as->sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }
//printf("PORT is %d\n", PORT);
    as->their_addr.sin_family = AF_INET;    // host byte order
    as->their_addr.sin_port = htons(PORT);  // short, network byte order
    as->their_addr.sin_addr = *((struct in_addr *)as->he->h_addr);
    memset(&(as->their_addr.sin_zero), '\0', 8);  // zero the rest of the struct

#endif // USE_UDP
    return as;
}
