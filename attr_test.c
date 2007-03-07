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
#include "atl.h"

#define FOSTERS_ATOM ATL_CHAR_CONS('F', 'O', 'S', 'T')
#define RED_STRIPE_ATOM ATL_CHAR_CONS('R','e','d','S')
#define MOOSEHEAD_ATOM ATL_CHAR_CONS('M','o','o','s')
#define YUENGLING_ATOM ATL_CHAR_CONS('Y','U','E','N')
#define CARTA_BLANCA_ATOM ATL_CHAR_CONS('C','A','R','T')
#define MILLER_LITE_ATOM ATL_CHAR_CONS('Y','U','C','K')
#define URQUEL_ATOM ATL_CHAR_CONS('U','R','Q','U')

int
main()
{
    attr_list list = create_attr_list();
    attr_list list2 = create_attr_list();
    char *string;
    char *string2;

    attr_list al = create_attr_list();
    attr_list al2 = create_attr_list();

    set_attr (al, 2000, Attr_String, "test string");
    set_attr (al, 2001, Attr_Int4, (attr_value)2001);

    attr_add_list (al, al2);
    attr_list_to_string (al);
    dump_attr_list (al);
    free_attr_list(al);
    free_attr_list(al2);
    
    set_attr_atom_and_string("Fosters", FOSTERS_ATOM);
    set_attr_atom_and_string("IP_ADDR", ATL_CHAR_CONS('C','I','P','A'));
    set_attr_atom_and_string("Red Stripe", RED_STRIPE_ATOM);
    set_attr_atom_and_string("Moosehead", MOOSEHEAD_ATOM);
    set_attr_atom_and_string("Yuengling", YUENGLING_ATOM);
    set_attr_atom_and_string("Carta Blanca", CARTA_BLANCA_ATOM);
    set_attr_atom_and_string("Miller Lite", MILLER_LITE_ATOM);
    set_attr_atom_and_string("Pilsner Urquel", URQUEL_ATOM);

    add_attr(list, ATL_CHAR_CONS('C','I','P','A'), Attr_Int4, (attr_value) 
	     (((unsigned int)130<<24) + ((unsigned int) 207<<16) +((unsigned int)5 << 8) + 68));
    add_attr(list, FOSTERS_ATOM, Attr_Int4, (attr_value) 2);
    add_attr(list, RED_STRIPE_ATOM, Attr_String, (attr_value) strdup("Uno mas"));
    add_double_attr(list, URQUEL_ATOM, 3.14159);
    dump_attr_list(list);
/*    add_attr(list2, FOSTERS_ATOM, Attr_Int4, (attr_value) 4);*/
    attr_add_list(list, list2);
    dump_attr_list(list);
    printf("stringified is %s\n", attr_list_to_string(list2));
    putenv("ATL_BASE64_STRINGS=1");
    string = attr_list_to_string(list);
    printf("stringified version is >%s<\n", string);
    free_attr_list(list);
    list2 = attr_list_from_string(string);
    free(string);
    dump_attr_list(list2);
    printf("\n");
    list = attr_copy_list(list2);
    free_attr_list(list2);
    dump_attr_list(list);
    printf("\n");
    free_attr_list(list);

    list = create_attr_list();
    list2 = create_attr_list();

    add_attr(list, FOSTERS_ATOM, Attr_Int4, (attr_value) 2);
    add_attr(list, MOOSEHEAD_ATOM, Attr_Int4, (attr_value) 20);
    add_attr(list, RED_STRIPE_ATOM, Attr_String, (attr_value) strdup("Uno mas"));
    add_attr(list, YUENGLING_ATOM, Attr_String, (attr_value) strdup("Cervezas frias, por favor"));

    add_attr(list2, MOOSEHEAD_ATOM, Attr_Int4, (attr_value) 20);
    add_attr(list2, FOSTERS_ATOM, Attr_Int4, (attr_value) 2);
    add_attr(list2, YUENGLING_ATOM, Attr_String, (attr_value) strdup("Cervezas frias, por favor"));
    add_attr(list2, RED_STRIPE_ATOM, Attr_String, (attr_value) strdup("Uno mas"));

    dump_attr_list(list);
    printf("\n");
    dump_attr_list(list2);
    printf("\n");

    string = attr_list_to_string(list);
    string2 = attr_list_to_string(list2);
    
    printf ("string [ %s ]\n", string);
    printf ("string2 [ %s ]\n", string2);

    printf ("strcmp returned %d\n", strcmp(string,string2));
    free_attr_list(list2);
    free_attr_list(list);
    free(string); free(string2);

    list = create_attr_list();
    list2 = create_attr_list();

    add_attr(list, 130, Attr_Int4, (attr_value) 130);
    add_attr(list, 120, Attr_Int4, (attr_value) 120);
    add_attr(list, 110, Attr_Int4, (attr_value) 110);
    add_attr(list, 230, Attr_String, (attr_value) strdup("Uno mas"));
    add_attr(list, 220, Attr_String, (attr_value) strdup("Cervezas frias, por favor"));
    add_attr(list, 210, Attr_String, (attr_value) strdup("Mas tequila!"));

    add_attr(list2, 130, Attr_Int4, (attr_value) 130);
    add_attr(list2, 110, Attr_Int4, (attr_value) 110);
    add_attr(list2, 120, Attr_Int4, (attr_value) 120);

    add_attr(list2, 230, Attr_String, (attr_value) strdup("Uno mas"));
    add_attr(list2, 210, Attr_String, (attr_value) strdup("Mas tequila!"));
    add_attr(list2, 220, Attr_String, (attr_value) strdup("Cervezas frias, por favor"));

    dump_attr_list(list);
    printf("\n");
    dump_attr_list(list2);
    printf("\n");

    string = attr_list_to_string(list);
    string2 = attr_list_to_string(list2);
    
    printf ("string [ %s ]\n", string);
    printf ("string2 [ %s ]\n", string2);

    printf ("strcmp returned %d\n", strcmp(string,string2));
    free_attr_list(list2);
    free_attr_list(list);

    /*    list = create_attr_list();
    list2 = create_attr_list();
    */
    list = attr_list_from_string(string);
    list2 = attr_list_from_string(string2);

    dump_attr_list(list);
    printf("\n");
    dump_attr_list(list2);
    printf("\n");

    free_attr_list(list2);
    free_attr_list(list);
    free(string); free(string2);
    return 0;
}
