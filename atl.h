#ifndef ATL_H
#define ATL_H
#if defined(FUNCPROTO) || defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
#ifndef ARGS
#define ARGS(args) args
#endif
#ifndef	NULL
#define NULL	((void *) 0)
#endif
#else
#ifndef ARGS
#define ARGS(args) (/*args*/)
#endif
#ifndef	NULL
#define NULL	0
#endif
#endif
typedef int atom_t;

/* opaque type for atom server handle */
typedef struct _atom_server *atom_server;

extern
atom_t
atom_from_string ARGS((atom_server as, char *str));

extern
char *
string_from_atom ARGS((atom_server as, atom_t atom));

typedef enum {
    no_atom_cache, atom_cache, prefill_atom_cache
} atom_cache_type;

extern
atom_server
init_atom_server ARGS((atom_cache_type cache_style));

extern
char *
get_server_id ARGS((atom_server as));

typedef enum _attr_value_type { Attr_Undefined, Attr_Int4, Attr_Int8, 
				Attr_String, Attr_Opaque, Attr_Atom, Attr_List } attr_value_type;

/* opaque type for attr_lists */
typedef struct _attr_list_struct *attr_list;

typedef void *attr_value;

typedef struct attr_opaque {
    int length;
    void *buffer;
} attr_opaque, *attr_opaque_p;

typedef struct attr {
#ifdef STRING_ATOMS
    char *attr_id;
#else
    atom_t attr_id;
#endif
    attr_value_type val_type;
    attr_value value;
} attr, *attr_p;

typedef struct xmit_attr_s {
    atom_t	attr_name;
    attr_value_type attr_type;
    atom_t	attr_atom_val;
    char *	attr_string_val;
} xmit_attr, *xmit_attr_ref;

typedef struct xmit_object_s {
    int		attr_count;
    xmit_attr	*attrs;
} xmit_object, *xmit_object_ref;


/* operations on attr_lists */
extern attr_list create_attr_list();

extern void free_attr_list ARGS((attr_list list));

extern void add_ref_attr_list ARGS((attr_list list));

extern attr_list attr_join_lists ARGS((attr_list list1, attr_list list2));

extern attr_list attr_add_list ARGS((attr_list list1, attr_list list2));

extern int add_attr ARGS((attr_list attrs, atom_t attr_id, 
			  attr_value_type val_type,
			  attr_value value ));

extern int set_attr ARGS((attr_list attrs, atom_t attr_id, 
			  attr_value_type val_type,
			  attr_value value ));

extern int replace_attr ARGS((attr_list attrs, atom_t attr_id, 
			      attr_value_type val_type, attr_value value ));

extern int query_attr ARGS(( attr_list attrs, atom_t attr_id, 
			    attr_value_type *val_type_p, attr_value *value_p));

extern void dump_attr_list ARGS(( attr_list attrs ));

extern
atom_t
attr_atom_from_string ARGS((const char *str));

extern
char *
attr_string_from_atom ARGS((atom_t atom));

extern int attr_count ARGS((attr_list attrs));

extern attr_p get_attr ARGS((attr_list attrs, int index));

extern
char *
attr_list_to_string ARGS((attr_list attrs));

extern
attr_list
attr_list_from_string ARGS((char * str));

extern
xmit_object
pack_attr_list (attr_list attrs);

extern
void
pack_attr_list_1 (attr_list attrs,
		  xmit_object xo);

extern
attr_list
unpack_attr_list (xmit_object xo);

extern
void
unpack_attr_list_1 (xmit_object xo,
		    attr_list attrs);

extern
void
unpack_attr_list_2 (int xmit_attr_count,
		    xmit_attr *xmit_list, 
		    attr_list attrs);

extern
int
attr_list_subset (attr_list l1, attr_list l2);

#endif
