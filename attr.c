#include "config.h"
#include "atl.h"

#ifndef MODULE
#include <config.h>
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#else
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#endif
#include <assert.h>
#include <atl.h>
//#include <tclHash.h>
#include <unix_defs.h> 
//#define MAXATTR 64
#include <unix_defs.h>
#else
#ifndef __KERNEL__
#define __KERNEL__
#endif

#include "kernel/katl.h"
#include "kernel/library.h"
#include <linux/ctype.h>
#include "kernel/kernel_defs.h"

#endif
#include "assert.h"

/*
typedef struct _attr_sublist_struct {
    Tcl_HashTable attr_hash_table;
    int ref_count;
    struct attr attribute[MAXATTR];
} attr_sublist_struct; 
*/
atom_server global_as = NULL;

static
void
init_global_atom_server(asp)
atom_server *asp;
{
    char *addr_str = NULL;
    char var_str[60];

    if (*asp != NULL) return;
    /*
     * for notes on my we're doing funny things with the "global" value,
     * global_as, see the comments in gen_info.c.  This is similar.
     */
    sprintf(var_str, "ATOM_SERVER_ADDRESS_%lx", (long) getpid());
#ifndef HAVE_WINDOWS_H
    addr_str = getenv(var_str);
#else
    {
        int ret;
        char buffer[60];
        ret = GetEnvironmentVariable(var_str, buffer, sizeof(buffer));
        if (ret != 0) {
            strcpy(var_str, buffer);
            addr_str = &var_str[0];
        }
    }
#endif
    if (addr_str == NULL) {
        char addr_tmp[64];
        int value;
        *asp = init_atom_server(prefill_atom_cache);
#ifndef HAVE_WINDOWS_H
#ifdef MODULE
	sprintf(addr_tmp, "%lx", (long)*asp);
	addr_str = strdup(addr_tmp);
	setenv(var_str, addr_str, 1);
#else
	sprintf(addr_tmp, "%s=%lx", var_str, (long)*asp);
	addr_str = strdup(addr_tmp);
	value = putenv(addr_str);
	if (value != 0) perror("putenv failed\n");
#endif
#else
        sprintf(addr_tmp, "%lx", (long)*asp);
        addr_str = strdup(addr_tmp);
        value = SetEnvironmentVariable(var_str, addr_str);
#endif
    } else {
        sscanf(addr_str, "%lx", (long*)asp);
        *(long*)asp = strtol((char *)addr_str, NULL, 16); 
    }
}

extern attr_list
create_attr_list(void)
{
attr_list list;
 list = malloc(sizeof( struct _attr_sublist_struct));
    init_global_atom_server(&global_as);
    list->ref_count = 0;
    Tcl_InitHashTable(&(list->attr_hash_table), TCL_ONE_WORD_KEYS);
    return list;
}

extern void
add_ref_attr_list(list)
attr_list list;
{
//   if(list) list->ref_count++;

}

extern attr_list
attr_join_lists(attr_list list1, attr_list list2)
{
 // Copy from list2 to list1
  
 int i;
if(list1 == NULL) return(attr_join_lists(list2, list1));
if(list2 == NULL) return list1;
 if(list1->ref_count + list2->ref_count > MAXATTR) exit(1); // Bail out if too big
 for (i = 0; i < list2->ref_count; i++) list1->attribute[i+list1->ref_count] = list2->attribute[i];
 
 list1->ref_count += list2->ref_count;
 return list1;
}

extern attr_list
attr_add_list(attr_list list1, attr_list list2)
{
 // Since we have got rid of list_of_sublist data structure 
 return attr_join_lists(list2, list1);

}

extern void attr_merge_lists(attr_list list1, attr_list list2)
{

 attr_p attr_guy;
 int i;
 int c = attr_count (list2);

 for (i = 0; i < c; i++)
   {
     attr_guy = get_attr (list2, i);
     if (attr_guy->val_type == Attr_String)
       {
         char* s = strdup ((char*)attr_guy->value);
         set_attr (list1, attr_guy->attr_id, attr_guy->val_type, s);
       }
     else
         set_attr (list1, attr_guy->attr_id, attr_guy->val_type, attr_guy->value);
 } 

}

extern int add_attr(attr_list list, atom_t attr_id, attr_value_type val_type, attr_value value)
{

 Tcl_HashEntry *entry = NULL;
 int new;

 list->attribute[list->ref_count].attr_id = attr_id;
 list->attribute[list->ref_count].val_type = val_type;
 list->attribute[list->ref_count].value = value;

 entry = Tcl_CreateHashEntry(&(list->attr_hash_table), attr_id, &new);
 Tcl_SetHashValue(entry, list->ref_count);
 list->ref_count++;

//printf("refocunt is %d\n", list->ref_count);
 return new;

}

extern int set_attr(attr_list list, atom_t attr_id, attr_value_type val_type, attr_value value)
{
   if (replace_attr(list, attr_id, val_type, value) == 0) {
        return add_attr(list, attr_id, val_type, value);
    }
    return 1;
}

extern int replace_attr(attr_list list, atom_t attr_id, attr_value_type val_type, attr_value value)
{

 Tcl_HashEntry *entry = NULL;
 
 entry = Tcl_FindHashEntry(&list->attr_hash_table, attr_id);
 
 if(entry){
	long index = (long) Tcl_GetHashValue(entry);
	list->attribute[index].val_type = val_type;
	list->attribute[index].value = value;
	return 1;
 }
 return 0;

}

extern int query_attr(attr_list list, atom_t attr_id, attr_value_type *val_type_p, attr_value *value_p)
{
   Tcl_HashEntry *entry = NULL;

   entry = Tcl_FindHashEntry(&list->attr_hash_table,
           attr_id);
   if (entry) {
    long index = (long) Tcl_GetHashValue(entry);
    if (val_type_p != NULL) 
     *val_type_p = list->attribute[index].val_type;
    if (value_p != NULL) {
     if ((sizeof(long) != 4) && (list->attribute[index].val_type == Attr_Int4))
         *((int*)value_p) = (int)(long)list->attribute[index].value;
     else 
         *value_p = list->attribute[index].value;
   }
   return 1;
  }
 return 0;
}

extern void dump_attr(attr_list list)
{
 int i, j;
 char *attr_name, *atom_str;

 init_global_atom_server(&global_as);
 for(i = 0; i < list->ref_count; i++){
   attr_name = string_from_atom(global_as, list->attribute[i].attr_id);  
   atom_str = NULL;

   if(attr_name == 0)
       sprintf(attr_name ,"<null attr name>");

          switch (list->attribute[i].val_type) {
        case Attr_Undefined:
            printf("    { %s, Undefined, Undefined }\n", attr_name);
            break;
        case Attr_Int4:
            printf("    { %s, Attr_Int4, %ld }\n", attr_name,
                   (long) list->attribute[i].value);
            break;
        case Attr_Int8:
            printf("    { %s, Attr_Int4, %ld }\n", attr_name,
                   (long) list->attribute[i].value);
            break;
        case Attr_String:
            if (((char*)list->attribute[i].value) != NULL) {
                printf("    { %s, Attr_String, %s }\n", attr_name,
                       (char *) list->attribute[i].value);
            } else {
                printf("    { %s, Attr_String, NULL }\n", attr_name);
            }
            break;
        case Attr_Opaque:
            if (((char*)list->attribute[i].value) != NULL) {
                int j;
                attr_opaque_p o =
                    (attr_opaque_p) list->attribute[i].value;
                printf("    { %s, Attr_Opaque, \"", attr_name);
                for (j=0; j< o->length; j++) {
                    printf("%c", ((char*)o->buffer)[j]);
                }
                printf("\"\n            <");
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
                                     (atom_t) (long)list->attribute[i].value);
            printf("    { %s, Attr_Atom, %s }\n", attr_name,
                   (char *) atom_str);
            break;
        default:
            assert(0);
        }
        free(attr_name);
        if (atom_str)
            free(atom_str);
    
 }
 
 
}

static void dump_attr_sublist(attr_list list, int indent)
{
 int i, j;
 char *attr_name, *atom_str;

 init_global_atom_server(&global_as);
  if(list == NULL) {
   printf("[NULL]\n");
   return;
  }
 for(i = 0; i < list->ref_count; i++){
   attr_name = string_from_atom(global_as, list->attribute[i].attr_id);  
   atom_str = NULL;

   if(attr_name == 0)
       sprintf(attr_name ,"<null attr name>");
   for(j = 0; j < indent; j++) printf("    ");

          switch (list->attribute[i].val_type) {
        case Attr_Undefined:
            printf("    { %s, Undefined, Undefined }\n", attr_name);
            break;
        case Attr_Int4:
            printf("    { %s, Attr_Int4, %ld }\n", attr_name,
                   (long) list->attribute[i].value);
            break;
        case Attr_Int8:
            printf("    { %s, Attr_Int4, %ld }\n", attr_name,
                   (long) list->attribute[i].value);
            break;
        case Attr_String:
            if (((char*)list->attribute[i].value) != NULL) {
                printf("    { %s, Attr_String, %s }\n", attr_name,
                       (char *) list->attribute[i].value);
            } else {
                printf("    { %s, Attr_String, NULL }\n", attr_name);
            }
            break;
        case Attr_Opaque:
            if (((char*)list->attribute[i].value) != NULL) {
                int j;
                attr_opaque_p o =
                    (attr_opaque_p) list->attribute[i].value;
                printf("    { %s, Attr_Opaque, \"", attr_name);
                for (j=0; j< o->length; j++) {
                    printf("%c", ((char*)o->buffer)[j]);
                }
                printf("\"\n            <");
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
                                     (atom_t) (long)list->attribute[i].value);
            printf("    { %s, Attr_Atom, %s }\n", attr_name,
                   (char *) atom_str);
            break;
        default:
            assert(0);
        }
        free(attr_name);
        if (atom_str)
            free(atom_str);
    
 }

}

extern void dump_attr_list(attr_list list)
{
 init_global_atom_server(&global_as);
 dump_attr_sublist(list, 0);
}

static void internal_dump_attr_list(attr_list list, int indent)
{
 return;
}

static char *strdqcat( char *str, int *size_p, char *str2)
{
int str2_len = strlen(str2);
 
   str = realloc(str, *size_p + str2_len + 1);
    strcpy(str + *size_p, str2);
    (*size_p) += str2_len;
    return str;
}

static char *strdcat(char *str, int *size_p, char *str2)
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


static char *add_list_to_string(char *str, int *size_p, struct _attr_sublist_struct list)
{
 int i, pointer=0;
 init_global_atom_server(&global_as);
    for (i = 0; i < attr_count(&list); i++) {
        attr_p tmp = get_attr(&list, i); 
        char *atom_string;
        int value; 
        
        sprintf(str+pointer, "{%s", atom_string=string_from_atom(global_as, tmp->attr_id));
        pointer += strlen(atom_string)+1;
        free(atom_string);
        switch(tmp->val_type){
         case Attr_Undefined:
	       sprintf(str+pointer, ",U,");
	       pointer += 3;
	       break;
         case Attr_Int4:
	       value = tmp->value;
	       pointer += sprintf(str+pointer,",4,%d", value);
	       break; 
         case Attr_Int8:
	       pointer += sprintf(str+pointer,",8,%ld", (long)tmp->value);
	       break; 
	 case Attr_String:
		pointer += sprintf(str+pointer, ",S%d,", strlen(tmp->value));
		pointer += sprintf(str+pointer, "%s", (char *)tmp->value);
		break;
	 case Attr_Opaque:
	      assert(0);	
	 case Attr_Atom:
	       pointer += sprintf(str+pointer,",4,%d", (int)(long)tmp->value);
	       break; 
         default:
	      assert(0);	
        }
      sprintf(str+pointer, "},");
      pointer += 2;
    }
 return str;
}

/*
static char *add_list_to_string(char *str, int *size_p, attr_list list)
{
int i;
int size;

size = list->ref_count;

    init_global_atom_server(&global_as);
    for (i = 0; i < attr_count(list); i++) {
        attr_p tmp = get_attr(list, i);
        char str_tmp[128];
        char *atom_string;
        memset(str_tmp, 0, sizeof(str_tmp));

#ifdef SHARED_ATTR_NUMS
        sprintf(str_tmp, "{%d", tmp->attr_id);
#else
        sprintf(str_tmp, "{%s", atom_string =
                string_from_atom(global_as, tmp->attr_id));
        free(atom_string);
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
        default:
            printf("Got this..%d\n",tmp->val_type);
            assert(0);
        }
        str = strdqcat(str, size_p, "},");

    }

    return str;

}
*/

static int add_list_from_string(char *str, attr_list list)
{
 while (1) {
        char *first_comma = strchr(str, ',');
        char *value = first_comma ? strchr(first_comma+1, ',') : NULL;
        char *end = NULL;
        int attr_id, length;
        attr_value_type val_type = Attr_Undefined;
        attr_value val = 0;
        int int_value;
        long long_value;

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
            if (sscanf(value, ",%d", &int_value) != 1) return 0;
            val = (attr_value)int_value;
            end = strchr(value+1, ',') + 1;
            if (end == (char*)1) end = value + strlen(value);
            break;
        case '8':
            val_type = Attr_Int8;
            if (sscanf(value, ",%ld", &long_value) != 1) return 0;
            val = (attr_value)long_value;
            end = strchr(value+1, ',') + 1;
            if (end == (char*)1) end = value + strlen(value);
            break;
        case 'A':
            val_type = Attr_Atom;
            if (sscanf(value, ",%d", &int_value) != 1) return 0;
            val = (attr_value)int_value;
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
                    unsigned char       byte;
                    byte = (unsigned char) (hex2byte (value[0]) << 4);
                    byte |= hex2byte (value [1]);
                    ((char*)o->buffer)[i] = byte;
                    value += 2;
                }
free(o->buffer);
free(o);
            }
            end = value + 2;
            break;
        }
        default:
            assert(0);
        }
        add_attr(list, attr_id, val_type, val);
if(sizeof(val) == length+1) free(val);
        str = end;
    }
}

extern char *attr_list_to_string(attr_list attrs)
{
    int size = 0;
    return(add_list_to_string(malloc(128*attrs->ref_count), &size, *attrs));
}

extern char *attrATTR_list_to_string(attr_list attrs)
{
 char *retstr, *retdstr;
 int size;

 retdstr = attr_list_to_string(attrs);
 size = strlen(retdstr);
 retstr = (char *)malloc(sizeof(char)*(size+7));
 sprintf(retstr, "ATTR%04d%s", size, retdstr);
 free(retdstr);
 return retstr;
}
/*
	switch(*(first_comma+1)) {
	case 'U':
	    val_type = Attr_Undefined;
	    end = value + 2;
	    break;
	case '4':
	    val_type = Attr_Int4;
	    val = (attr_value) atoi(value+1);
	    end = strchr(value+1, ',') + 1;
	    if (end == (char*)1) end = value + strlen(value);
	    break;
	case '8':
	    val_type = Attr_Int8;
            long_value = atoi(value+1);
	    val = (attr_value)long_value;
	    end = strchr(value+1, ',') + 1;
	    if (end == (char*)1) end = value + strlen(value);
	    break;
	case 'A':
	    val_type = Attr_Atom;
            
	    val = (attr_value)atoi(value+1);
	    end = strchr(value+1, ',') + 1;
	    if (end == (char*)1) end = value + strlen(value);
	    break;
	case 'S':
	    val_type = Attr_String;
            length = atoi(first_comma+2);
	    end = value+1+length;
	    val = malloc(length+1);
	    strncpy(val, value+1, length);
	    ((char*)val)[length] = 0;  terminate string 
	    end += 2;
	    break;
	case 'O': {
	    int len;
            len = atoi(first_comma+2);
	    while (*value != ',') value++;  skip to start of buffer 
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
*/

extern attr_list attr_list_from_string (char *str)
{
    attr_list ret_val;
    ret_val = create_attr_list();
    if (add_list_from_string(str, ret_val) != 1) {
        free_attr_list(ret_val);
    }
    return ret_val;
}

extern atom_t attr_atom_from_string(const char *str)
{
     init_global_atom_server(&global_as);
    return atom_from_string(global_as, (char*)str);
//     return set_attr_atom_and_string(str);
}



extern atom_t set_attr_atom_and_string(const char *str, int no)
{

   int ret_val;

     init_global_atom_server(&global_as);
     do{
      ret_val = set_string_and_atom(global_as, (char *)str, no++);
     }while(ret_val == 0);
     return ret_val;

}


extern char *attr_string_from_atom(atom_t atom)
{
 init_global_atom_server(&global_as);
    return string_from_atom(global_as, atom);
}

extern int attr_count(attr_list list)
{

return list->ref_count;

}

extern attr_p get_attr(attr_list list, int index)
{

 if(index >= 0 && index < MAXATTR) 
  return &list->attribute[index];
 return NULL;

}

void free_attr_list(attr_list list)
{
int i;

 if(list){
//   Tcl_DeleteHashTable(&(list->attr_hash_table));
   free(list);
 }

}

extern xmit_object pack_attr_list(attr_list attribute_list)
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

extern void pack_attr_list_1(attr_list attribute_list, xmit_object xo)
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

extern attr_list unpack_attr_list(xmit_object xo)
{
attr_list al;
 al = create_attr_list();
  unpack_attr_list_2 (xo.attr_count, xo.attrs, al);
  return al;
}

extern void unpack_attr_list_1(xmit_object xo, attr_list al)
{
 unpack_attr_list_2 (xo.attr_count, xo.attrs, al);
}

extern void unpack_attr_list_2(int xmit_count, xmit_attr *xmit_list, attr_list al)
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

extern int compare_attr_p_by_val(attr_p a1, attr_p a2)
{
int eq = 0;

  if (a1 == a2)
    return 1;

  if (a1->val_type == a2->val_type)
    {
      if (a1->val_type == Attr_Int4 ||
          a1->val_type == Attr_Int8)
        {
          eq = ATTR_INT4_8_EQ(a1,a2);
        }
      else if (a1->val_type == Attr_String)
        {
          eq = ATTR_STRING_EQ(a1,a2);
        }
      else if (a1->val_type == Attr_Atom)
        {
          eq = ATTR_ATOM_EQ(a1,a2);
        }
      else
        {
          eq = 1;
        }

    }

  return eq;
}

extern int compare_attr_p_xmit_attr_by_val(attr_p ap, xmit_attr_ref xa)
{
 int eq = 0;

  if (ap->val_type == xa->attr_type)
    {
      if (ap->val_type == Attr_Int4 || ap->val_type == Attr_Int8)
        {
          eq = ATTR_INT4_8_XMIT_ATTR_EQ(ap,xa);
        }
      else if (ap->val_type == Attr_String)
        {
          eq = ATTR_STRING_XMIT_ATTR_EQ(ap,xa);
        }
      else if (ap->val_type == Attr_Atom)
        {
          eq = ATTR_ATOM_XMIT_ATTR_EQ(ap,xa);
        }
      else
        {
          eq = 1;
        }

    }
  return eq;
}

extern int attr_list_subset(attr_list l1, attr_list l2)
{
 int i, j, keep_going = 1;
  int l1_count;
  int l2_count;
  attr_p l1_tmp, l2_tmp;

  l1_count = attr_count (l1);
  l2_count = attr_count (l2);

  if (l2_count < l1_count) return 0;

  for (i = 0; i < l1_count && keep_going; i++)
    {
      int found = 0;
      l1_tmp = get_attr (l1, i);

      for (j = 0; !found && j < l2_count; j++)
        {
          l2_tmp = get_attr (l2, j);

          found = compare_attr_p_by_val (l1_tmp, l2_tmp);
        }

      keep_going = found;
    }

  return keep_going;
}

extern int attr_list_subset_xmit_object(xmit_object xo, attr_list l1)
{
int i, j, keep_going, l1_count;
  attr_p l1_p;

  keep_going = 1;

  l1_count = attr_count (l1);
  if (l1_count < xo.attr_count) return 0;

  for (i = 0; keep_going && i < xo.attr_count; i++)
    {
      int found = 0;

      for (j = 0; !found && j < l1_count; j++)
        {
          l1_p = get_attr (l1, j);

          found = compare_attr_p_xmit_attr_by_val (l1_p, &xo.attrs[i]);
        }

      keep_going = found;
    }

  return keep_going;
}

extern void print_everything(attr_list list)
{
 int i;
 for (i = 0; i < list->ref_count ; i++){
  printf(" Attribute stored is %d, %s\n", list->attribute[i].attr_id, (char *)list->attribute[i].value);
 } 
}
