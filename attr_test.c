#include "config.h"
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif
#include <stdio.h>
#include <atl.h>

#define FOSTERS_ATOM ATL_CHAR_CONS('F', 'O', 'S', 'T')
#define RED_STRIPE_ATOM ATL_CHAR_CONS('R','e','d','S')
int
main()
{
    attr_list list = create_attr_list();
    attr_list list2 = create_attr_list();
    char *string;

    attr_list al = create_attr_list();
    attr_list al2 = create_attr_list();

    set_attr (al, 2000, Attr_String, "test string");
    set_attr (al, 2001, Attr_Int4, (attr_value)2001);

    attr_add_list (al, al2);
    attr_list_to_string (al);
    dump_attr_list (al);

    set_attr_atom_and_string("Fosters", FOSTERS_ATOM);
    set_attr_atom_and_string("IP_ADDR", ATL_CHAR_CONS('C','I','P','A'));
    set_attr_atom_and_string("Red Stripe", RED_STRIPE_ATOM);
    add_attr(list, ATL_CHAR_CONS('C','I','P','A'), Attr_Int4, (attr_value) 
	     (((unsigned int)130<<24) + ((unsigned int) 207<<16) +((unsigned int)5 << 8) + 68));
    add_attr(list, FOSTERS_ATOM, Attr_Int4, (attr_value) 2);
    add_attr(list, RED_STRIPE_ATOM, Attr_String, (attr_value) strdup("Uno mas"));
    dump_attr_list(list);
/*    add_attr(list2, FOSTERS_ATOM, Attr_Int4, (attr_value) 4);*/
    attr_add_list(list, list2);
    dump_attr_list(list);
    printf("stringified is %s\n", attr_list_to_string(list2));
    putenv("ATL_BASE64_STRINGS=1");
    string = attr_list_to_string(list);
    printf("stringified version is >%s<\n", string);
    list2 = attr_list_from_string(string);
    free(string);
    dump_attr_list(list2);
    printf("\n");

    return 0;
}
