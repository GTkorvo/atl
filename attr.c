#include <config.h>
#include <atl.h>
#include <tclHash.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

static
void
init_global_atom_server(asp)
atom_server *asp;
{
    char *addr_str;
    char var_str[60];
    
    if (*asp != NULL) return;
    /*
     * for notes on my we're doing funny things with the "global" value, 
     * global_as, see the comments in gen_info.c.  This is similar.
     */
    sprintf(var_str, "ATOM_SERVER_ADDRESS_%lx", (long) getpid());
    if ((addr_str = getenv(var_str)) == NULL) {
	char addr_tmp[64];
	*asp = init_atom_server(prefill_atom_cache);
	printf("New global AS = %lx addr = %lx\n", (long)*asp, (long)asp);
	sprintf(addr_tmp, "%s=%lx", var_str, (long)*asp);
	addr_str = strdup(addr_tmp);
	putenv(addr_str);
	printf("after putenv, value is %s\n", getenv(var_str));
    } else {
	sscanf(addr_str, "%lx", (long*)asp);
	printf("Set global AS = %lx addr = %lx\n", (long)*asp, (long)asp);
    }
}
/* operations on attr_lists */
extern attr_list
create_attr_list()
{
    attr_list list = malloc(sizeof(attr_list_struct));

    init_global_atom_server(&global_as);
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

    init_global_atom_server(&global_as);
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
    init_global_atom_server(&global_as);
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
    init_global_atom_server(&global_as);
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
	case Attr_Opaque:
	    if (((char*)list->l.list.attributes[i].value) != NULL) {
		int j;
		attr_opaque_p o = 
		    (attr_opaque_p) list->l.list.attributes[i].value;
		printf("    { %s, Attr_Opaque, \"", attr_name);
		for (j=0; j< o->length; j++) {
		    printf("%c", ((char*)o->buffer)[j]);
		}
		printf("\"\n		<");
		for (j=0; j< o->length; j++) {
		    printf(" %02x", ((unsigned char*)o->buffer)[j]);
		}
		printf(">}\n");
	    } else {
		printf("    { %s, Attr_Opaque, NULL }\n", attr_name);
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
    
    init_global_atom_server(&global_as);
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

static char*
strdqcat(str, size_p, str2)
char *str;
int *size_p;
char *str2;
{
    int str2_len = strlen(str2);
    str = realloc(str, *size_p + str2_len + 1);
    strcpy(str + *size_p, str2);
    (*size_p) += str2_len;
    return str;
}

static char*
strdcat(str, size_p, str2)
char *str;
int *size_p;
char *str2;
{
    return strdqcat(str, size_p, str2);
}

static const char xchars [] = "0123456789abcdef";

#define nibble2hex(val) (xchars[val & 0x0f])

static
unsigned char
hex2byte (char c)
{
    if (isdigit ((int)c))
	return (unsigned char) (c - '0');
    else if (islower ((int)c))
	return (unsigned char) (10 + c - 'a');
    else
	return (unsigned char) (10 + c - 'A');
}

static char *
add_list_to_string(str, size_p, list)
char *str;
int *size_p;
attr_list list;
{
    int i;
    init_global_atom_server(&global_as);
    for (i = 0; i < attr_count(list); i++) {
	attr_p tmp = get_attr(list, i);
	char str_tmp[128];
	memset(str_tmp, 0, sizeof(str_tmp));
	if (tmp->val_type == Attr_List) continue;
#ifdef SHARED_ATTR_NUMS
	sprintf(str_tmp, "{%d", tmp->attr_id);
#else
	sprintf(str_tmp, "{%s", string_from_atom(global_as, tmp->attr_id));
#endif
	str = strdcat(str, size_p, str_tmp);
	switch(tmp->val_type) {
	case Attr_Undefined:
	    str = strdcat(str, size_p, ",U,");
	    break;
	case Attr_Int4:
	    sprintf(str_tmp, ",4,%d", (int) (long)tmp->value);
	    str = strdcat(str, size_p, str_tmp);
	    break;
	case Attr_Int8:
	    sprintf(str_tmp, ",8,%ld", (long) tmp->value);
	    str = strdcat(str, size_p, str_tmp);
	    break;
	case Attr_String:
	    {
		char tmp_str[24];
		sprintf(tmp_str, ",S%d,", (int)strlen(tmp->value));
		str = strdcat(str, size_p, tmp_str);
		str = strdcat(str, size_p, (char*)tmp->value);
	    }
	    break;
	case Attr_Opaque: {
	    attr_opaque_p o = (attr_opaque_p) tmp->value;
	    if (o == NULL) {
		str = strdcat(str, size_p, ",O0,");
	    } else {
		char tmp3[3];
		int i;
		sprintf(str_tmp, ",O%d,", o->length);
		str = strdcat(str, size_p, str_tmp);
		tmp3[2] = 0;
		for (i=0; i<o->length; i++) {
		    tmp3[0] = nibble2hex(((char*)o->buffer)[i] >> 4);
		    tmp3[1] = nibble2hex(((char*)o->buffer)[i]);
		    str = strdcat(str, size_p, tmp3);
		}
	    }
	    break;
	}
	case Attr_Atom:
	    sprintf(str_tmp, ",A,%d", (int) (long)tmp->value);
	    str = strdcat(str, size_p, str_tmp);
	    break;
	case Attr_List:
	default:
	    assert(0);
	}
	str = strdqcat(str, size_p, "},");
    }
    return str;
}

static int
add_list_from_string(str, list)
char *str;
attr_list list;
{
    while (1) {
	char *first_comma = strchr(str, ',');
	char *value = first_comma ? strchr(first_comma+1, ',') : NULL;
	char *end = NULL;
	int attr_id, length;
	attr_value_type val_type = Attr_Undefined;
	attr_value val;
	
	if (*str == 0) return 1;   /* success */
	if (value == NULL) return 0;
#ifdef SHARED_ATTR_NUMS
	if (sscanf(str, "{%d,", &attr_id) != 1) return 0;
#else
	
	{ 
	    int len = first_comma - str - 1;
	    char * atom_name = malloc(len + 1);
	    strncpy(atom_name, (str+1), len);
	    atom_name[len] = 0;
	    attr_id = attr_atom_from_string(atom_name);
	    free(atom_name);
	}
#endif
	switch(*(first_comma+1)) {
	case 'U':
	    val_type = Attr_Undefined;
	    end = value + 2;
	    break;
	case '4':
	    val_type = Attr_Int4;
	    if (sscanf(value, ",%d", (int*)&val) != 1) return 0;
	    end = strchr(value+1, ',') + 1;
	    if (end == (char*)1) end = value + strlen(value);
	    break;
	case '8':
	    val_type = Attr_Int8;
	    if (sscanf(value, ",%ld", (long*)&val) != 1) return 0;
	    end = strchr(value+1, ',') + 1;
	    if (end == (char*)1) end = value + strlen(value);
	    break;
	case 'A':
	    val_type = Attr_Atom;
	    if (sscanf(value, ",%d", (int*)&val) != 1) return 0;
	    end = strchr(value+1, ',') + 1;
	    if (end == (char*)1) end = value + strlen(value);
	    break;
	case 'S':
	    val_type = Attr_String;
	    sscanf(first_comma+2, "%d", &length);
	    end = value+1+length;
	    val = malloc(length+1);
	    strncpy(val, value+1, length);
	    ((char*)val)[length] = 0; /* terminate string */
	    end += 2;
	    break;
	case 'O': {
	    int len;
	    if (sscanf(first_comma + 2, "%d", &len) != 1) return 0;
	    while (*value != ',') value++; /* skip to start of buffer */
	    value++;
	    val_type = Attr_Opaque;
	    if (len == 0) {
		val = (attr_value) NULL;
	    } else {
		attr_opaque_p o = malloc(sizeof(attr_opaque));
		int i;
		val = (attr_value)o;
		o->length = len;
		o->buffer = malloc(len);
		for (i=0 ; i < len; i++) {
		    unsigned char	byte;
		    byte = (unsigned char) (hex2byte (value[0]) << 4);
		    byte |= hex2byte (value [1]);
		    ((char*)o->buffer)[i] = byte;
		    value += 2;
		}
	    }
	    end = value + 2;
	    break;
	}    
	default:
	    assert(0);
	}
	add_attr(list, attr_id, val_type, val);
	str = end;
    }
}

extern
char *
attr_list_to_string(attrs)
attr_list attrs;
{
    int size = 0;
    return(add_list_to_string(malloc(1), &size, attrs));
}

extern
attr_list
attr_list_from_string(str)
char * str;
{
    attr_list ret_val = create_attr_list();
    if (add_list_from_string(str, ret_val) != 1) {
	free_attr_list(ret_val);
	return NULL;
    }
    return ret_val;
}

extern
 atom_t
attr_atom_from_string(str)
const char *str;
{
    init_global_atom_server(&global_as);
    return atom_from_string(global_as, (char*)str);
}

extern
char *
attr_string_from_atom(atom)
atom_t atom;
{
    init_global_atom_server(&global_as);
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
	    case Attr_Opaque: {
		attr_opaque_p o = 
		    (attr_opaque_p) list->l.list.attributes[i].value;
		if (o) {
		    free(o->buffer);
		    free(o);
		}
		break;
	    }
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
  attr_list al = create_attr_list();
  unpack_attr_list_2 (xo.attr_count, xo.attrs, al);
  return al;
}


extern
void
unpack_attr_list_1 (xmit_object xo, attr_list al)
{
  unpack_attr_list_2 (xo.attr_count, xo.attrs, al);
}

extern
void
unpack_attr_list_2 (int xmit_count,
		    xmit_attr *xmit_list,
		    attr_list al)
{
  int i;

  for (i = 0; i < xmit_count; i++)
    {
      if (xmit_list[i].attr_type == Attr_String)
	set_attr (al,
		  xmit_list[i].attr_name,
		  xmit_list[i].attr_type,
		  strdup ((char*)xmit_list[i].attr_string_val));
      else
	set_attr (al,
		  xmit_list[i].attr_name,
		  xmit_list[i].attr_type,
		  (void*)xmit_list[i].attr_atom_val);
    }
}


extern
int
compare_attr_p_by_val (attr_p a1, attr_p a2)
{
  int eq = 0;
  
  if (a1 == a2)
    return 1;

  if (a1->val_type == a2->val_type)
    {
      if (a1->val_type == Attr_Int4 ||
	  a1->val_type == Attr_Int8)
	{
	  eq = (a1->value == a2->value);
	}
      else if (a1->val_type == Attr_String)
	{
	  eq = (strcmp ((char*)a1->value, "*") == 0 ||
		strcmp ((char*)a2->value, "*") == 0 ||
		strcmp ((char*)a1->value, (char*)a2->value) == 0);
	}
      else if (a1->val_type == Attr_List)
	{
	  eq = attr_list_subset ((attr_list)a1->value,
				 (attr_list)a2->value);
	}
      else
	{
	  eq = 1;
	}

    }
  
  return eq;
}


extern
int
attr_list_subset (attr_list l1, attr_list l2)
{
  /*
   *  This function returns true if l1 is a subset of l2.  I
   *  define this as:  for each element of l1, there is an element
   *  of l2 with the same attribute name, attribute type, and attribute
   *  value.
   */
  int i, j, eq = 1;
  int l1_count;
  int l2_count;
  attr_p l1_tmp, l2_tmp;

  l1_count = attr_count (l1);
  l2_count = attr_count (l2);
  
  if (l2_count < l1_count) return 0;

  for (i = 0; i < l1_count && eq; i++)
    {
      
      l1_tmp = get_attr (l1, i);

      for (j = 0; j < l2_count; j++)
	{
	  l2_tmp = get_attr (l2, j);
	  
	  eq = compare_attr_p_by_val (l1_tmp, l2_tmp);
	}
    }

  return eq;
}
	
	
  

  

  
