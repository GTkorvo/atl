#include <config.h>
#include <atom.h>
#include <attr.h>
#include <tclHash.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <assert.h>
#include <unix_defs.h>

typedef struct _attr_sublist_struct {
    Tcl_HashTable attr_hash_table;
    int attr_count;
    attr_p attributes;
} attr_sublist_struct;

typedef struct _attr_list_of_lists_struct {
    int sublist_count;
    attr_list *lists;
} attr_list_of_lists_struct;

/* opaque type for attr_lists */
typedef struct _attr_list_struct {
    int list_of_lists;
    int ref_count;
    union {
	attr_list_of_lists_struct lists;
	attr_sublist_struct list;
    } l;
} attr_list_struct;

atom_server global_as = NULL;

/* operations on attr_lists */
extern attr_list
create_attr_list()
{
    attr_list list = malloc(sizeof(attr_list_struct));

    if (global_as == NULL) {
	global_as = init_atom_server(prefill_atom_cache);
    }
    list->list_of_lists = 0;
    list->ref_count = 1;
    Tcl_InitHashTable(&(list->l.list.attr_hash_table), TCL_ONE_WORD_KEYS);
    list->l.list.attr_count = 0;
    list->l.list.attributes = (attr_p) malloc(sizeof(attr));
    return list;
}

extern void
add_ref_attr_list(list)
attr_list list;
{
    if(list) {
	list->ref_count++;
    }
}

/* operations on attr_lists */
extern attr_list
attr_join_lists(list1, list2)
attr_list list1;
attr_list list2;
{
    attr_list list;

    if (list2 == NULL) {
	list1->ref_count++;
	return list1;
    }
    list = malloc(sizeof(attr_list_struct));

    if (global_as == NULL) {
	global_as = init_atom_server(prefill_atom_cache);
    }
    list->list_of_lists = 1;
    list->ref_count = 1;
    list->l.lists.lists = (attr_list *)malloc(sizeof(attr_list) *2);
    list->l.lists.sublist_count = 2;
    list->l.lists.lists[0] = list1;
    list1->ref_count++;
    list->l.lists.lists[1] = list2;
    list2->ref_count++;
    return list;
}

extern attr_list
attr_add_list(list1, list2)
attr_list list1;
attr_list list2;
{
    if (global_as == NULL) {
	global_as = init_atom_server(prefill_atom_cache);
    }
    if (list1->list_of_lists == 0) {
	return attr_join_lists(list1, list2);
    }
    list1->l.lists.lists = 
	   (attr_list *)realloc(list1->l.lists.lists, sizeof(attr_list) * 
				(list1->l.lists.sublist_count+1));
    list1->l.lists.lists[list1->l.lists.sublist_count] = list2;
    list2->ref_count++;
    list1->l.lists.sublist_count += 1;
    return list1;
}

extern int
add_attr(list, attr_id, val_type, value)
attr_list list;
atom_t attr_id;
attr_value_type val_type;
attr_value value;
{
    Tcl_HashEntry *entry = NULL;
    int new;

    list->l.list.attributes = (attr_p) realloc(list->l.list.attributes, sizeof(attr) *
					(list->l.list.attr_count + 1));
    list->l.list.attributes[list->l.list.attr_count].attr_id = attr_id;
    list->l.list.attributes[list->l.list.attr_count].val_type = val_type;
    list->l.list.attributes[list->l.list.attr_count].value = value;
    entry = Tcl_CreateHashEntry(&list->l.list.attr_hash_table, (char *) attr_id,
				&new);
    Tcl_SetHashValue(entry, (char *) list->l.list.attr_count);
    list->l.list.attr_count++;
    return new;
}

extern int 
set_attr(list, attr_id, val_type, value)
attr_list list;
atom_t attr_id;
attr_value_type val_type;
attr_value value;
{
    if (replace_attr(list, attr_id, val_type, value) == 0) {
	return add_attr(list, attr_id, val_type, value);
    }
    return 1;
}

extern int
replace_attr(list, attr_id, val_type, value)
attr_list list;
atom_t attr_id;
attr_value_type val_type;
attr_value value;
{
    Tcl_HashEntry *entry = NULL;

    entry = Tcl_FindHashEntry(&list->l.list.attr_hash_table, (char *) attr_id);

    if (entry) {
	long index = (long) Tcl_GetHashValue(entry);
	list->l.list.attributes[index].val_type = val_type;
	list->l.list.attributes[index].value = value;
	return 1;
    }
    return 0;
}

extern int
query_attr(list, attr_id, val_type_p, value_p)
attr_list list;
atom_t attr_id;
attr_value_type *val_type_p;
attr_value *value_p;
{
    Tcl_HashEntry *entry = NULL;

    if (list->list_of_lists) {
	int i;
	for (i=0; i< list->l.lists.sublist_count; i++) {
	    if (query_attr(list->l.lists.lists[i], attr_id, val_type_p,
			   value_p)) {
		return 1;
	    }
	}
	return 0;
    } else {
	entry = Tcl_FindHashEntry(&list->l.list.attr_hash_table,
				  (char *) attr_id);
	if (entry) {
	    long index = (long) Tcl_GetHashValue(entry);
	    if (val_type_p != NULL)
	      *val_type_p = list->l.list.attributes[index].val_type;
	    if (value_p != NULL)
	      *value_p = list->l.list.attributes[index].value;
	    return 1;
	}
	return 0;
    }
}

static void
internal_dump_attr_list	ARGS((attr_list list, int indent));

static void
dump_attr_sublist(list, indent)
attr_list list;
int indent;
{
    int i;
    if (list == NULL) {
	printf("[NULL]\n");
	return;
    }
    for (i = 0; i < list->l.list.attr_count; i++) {
	char *attr_name = string_from_atom(global_as,
					   list->l.list.attributes[i].attr_id);
	char *atom_str = NULL;
	int j;
	if (attr_name == NULL)
	    attr_name = "<null attr name>";
	for (j = 0; j < indent; j++) {
	    printf("    ");
	}
	switch (list->l.list.attributes[i].val_type) {
	case Attr_Undefined:
	    printf("    { %s, Undefined, Undefined }\n", attr_name);
	    break;
	case Attr_Int4:
	    printf("    { %s, Attr_Int4, %ld }\n", attr_name,
		   (long) list->l.list.attributes[i].value);
	    break;
	case Attr_Int8:
	    printf("    { %s, Attr_Int4, %ld }\n", attr_name,
		   (long) list->l.list.attributes[i].value);
	    break;
	case Attr_String:
	    if (((char*)list->l.list.attributes[i].value) != NULL) {
		printf("    { %s, Attr_String, %s }\n", attr_name,
		       (char *) list->l.list.attributes[i].value);
	    } else {
		printf("    { %s, Attr_String, NULL }\n", attr_name);
	    }
	    break;
	case Attr_Atom:
	    atom_str = string_from_atom(global_as,
				     (atom_t) (long)list->l.list.attributes[i].value);
	    printf("    { %s, Attr_Atom, %s }\n", attr_name,
		   (char *) atom_str);
	    break;
	case Attr_List:
	    printf("    { %s, Attr_List, ->\n", attr_name);
	    internal_dump_attr_list((attr_list) list->l.list.attributes[i].value,
				    indent+1);
	    for (j = 0; j< indent; j++) {
		printf("    ");
	    }
	    printf(" }\n");
	    break;
	default:
	    assert(0);
	}
	free(attr_name);
	if (atom_str)
	    free(atom_str);
    }
}

extern void
dump_attr_list(list)
attr_list list;
{
    internal_dump_attr_list(list, 0);
}

static void
internal_dump_attr_list	(list, indent)
attr_list list;
int indent;
{
    int i = 0;
    for (i = 0; i< indent; i++) {
	printf("    ");
    }
    if (list == NULL) {
	printf("[NULL]\n");
	return;
    }
    printf("[\n");
    if (!list->list_of_lists) {
	dump_attr_sublist(list, indent);
    } else {
	int i;
	for (i=0; i<list->l.lists.sublist_count; i++) {
	    dump_attr_sublist(list->l.lists.lists[i], indent);
	}
    }
    for (i=0; i< indent; i++) {
	printf("    ");
    }
    printf("]\n");
}

extern
 atom_t
attr_atom_from_string(str)
char *str;
{
    if (global_as == NULL) {
	global_as = init_atom_server(prefill_atom_cache);
    }
    return atom_from_string(global_as, str);
}

extern
char *
attr_string_from_atom(atom)
atom_t atom;
{
    if (global_as == NULL) {
	global_as = init_atom_server(prefill_atom_cache);
    }
    return string_from_atom(global_as, atom);
}

extern
int 
attr_count(list)
attr_list list;
{
    if (list == NULL) return 0;
    if (!list->list_of_lists) {
	return list->l.list.attr_count;
    } else {
	int count = 0;
	int i;
	for (i=0; i<list->l.lists.sublist_count; i++) {
	    count += attr_count(list->l.lists.lists[i]);
	}
	return count;
    }
}

extern
attr_p 
get_attr(list, index)
attr_list list;
int index;
{
    if (!list->list_of_lists) {
	return &list->l.list.attributes[index];
    } else {
	int i;
	for (i=0; i<list->l.lists.sublist_count; i++) {
	    int count = attr_count(list->l.lists.lists[i]);
	    if (index < count) {
		return get_attr(list->l.lists.lists[i], index);
	    } else {
		index -= count;
	    }
	}
	return NULL;
    }
}

void
free_attr_list(list)
attr_list list;
{
    if (list == NULL) return;
    list->ref_count--;
    if (list->ref_count > 0) return;
    if (list->list_of_lists) {
	int i;
	for (i=0; i<list->l.lists.sublist_count; i++) {
	    free_attr_list(list->l.lists.lists[i]);
	}
	free(list->l.lists.lists);
	free(list);
    } else {
	int i;
	for (i=0; i< list->l.list.attr_count; i++) {
#ifdef STRING_ATOMS
	    free(list->l.list.attributes[i].attr_id);
#endif
	    switch (list->l.list.attributes[i].val_type) {
	    case Attr_Undefined:
	    case Attr_Int4:
		break;
	    case Attr_Atom:
#ifdef STRING_ATOMS
		free((char *)list->l.list.attributes[i].value);
#endif
		break;
	    case Attr_String:
		free((char *)list->l.list.attributes[i].value);
		break;
	    case Attr_List:
		free_attr_list((attr_list)list->l.list.attributes[i].value);
		break;
	    default:
		assert(0);
	    }
	}
	Tcl_DeleteHashTable(&(list->l.list.attr_hash_table));
	if (list->l.list.attributes != NULL) {
	    free(list->l.list.attributes);
	}
	free(list);
    }
}


extern
xmit_object
pack_attr_list (attr_list attribute_list)
{
  attr_p tmp;
  xmit_object xo;
  int i;
 
  xo.attr_count = attr_count (attribute_list);
  xo.attrs = (xmit_attr_ref) malloc (sizeof (xmit_attr) * xo.attr_count);
  
  for (i = 0; i < xo.attr_count; i++)
    {
      tmp = get_attr (attribute_list, i);
      xo.attrs[i].attr_name = tmp->attr_id;
      xo.attrs[i].attr_type = tmp->val_type;
      if (tmp->val_type == Attr_String)
	{
	  xo.attrs[i].attr_atom_val = (long) 0;
	  xo.attrs[i].attr_string_val = (char*) tmp->value;
	}
      else
	{
	  xo.attrs[i].attr_atom_val = (long) tmp->value;
	  xo.attrs[i].attr_string_val = (char*) 0;
	}
    }

  return xo;
}

extern
void
pack_attr_list_1 (attr_list attribute_list, xmit_object xo)
{
  attr_p tmp;
  int i;
  int s = attr_count (attribute_list);

  xo.attrs = (xmit_attr_ref) 
    realloc (xo.attrs, sizeof (xmit_attr) * (xo.attr_count + s));
				      
  for (i = 0; i < s; i++)
    {
      tmp = get_attr (attribute_list, i);
      xo.attrs[xo.attr_count + i].attr_name = tmp->attr_id;
      xo.attrs[xo.attr_count + i].attr_type = tmp->val_type;
      if (tmp->val_type == Attr_String)
	{
	  xo.attrs[xo.attr_count + i].attr_atom_val = (long) 0;
	  xo.attrs[xo.attr_count + i].attr_string_val = (char*) tmp->value;
	}
      else
	{
	  xo.attrs[xo.attr_count + i].attr_atom_val = (long) tmp->value;
	  xo.attrs[xo.attr_count + i].attr_string_val = (char*) 0;
	}
    }
}


extern
attr_list
unpack_attr_list (xmit_object xo)
{
  int i;
  attr_list al = create_attr_list();

  for (i = 0; i < xo.attr_count; i++)
    {
      if (xo.attrs[i].attr_type == Attr_String)
	set_attr (al,
		  xo.attrs[i].attr_name,
		  xo.attrs[i].attr_type,
		  xo.attrs[i].attr_string_val);
      else
	set_attr (al,
		  xo.attrs[i].attr_name,
		  xo.attrs[i].attr_type,
		  (void*)xo.attrs[i].attr_atom_val);
    }

  return al;
}


extern
void
unpack_attr_list_1 (xmit_object xo, attr_list al)
{
  int i;

  for (i = 0; i < xo.attr_count; i++)
    {
      if (xo.attrs[i].attr_type == Attr_String)
	set_attr (al,
		  xo.attrs[i].attr_name,
		  xo.attrs[i].attr_type,
		  xo.attrs[i].attr_string_val);
      else
	set_attr (al,
		  xo.attrs[i].attr_name,
		  xo.attrs[i].attr_type,
		  (void*)xo.attrs[i].attr_atom_val);
    }
}
