#define PORT 8645

typedef struct _send_get_atom_msg {
    char *atom_string;
    atom_t atom;
} send_get_atom_msg, *send_get_atom_msg_ptr;

typedef struct _init_contact {
    int send_id;
    int send_list;
} init_contact_msg, *init_contact_msg_ptr;

typedef struct _provisional_use_msg {
    char *domain;
    long time;
} provisional_use_msg, *provisional_use_msg_ptr;

extern IOField Atom_send_get_msg_flds[];
extern IOField Atom_init_contact_msg_flds[];
extern IOField atom_provisional_use_msg_flds[];
