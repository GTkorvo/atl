#include <stdio>
#include <atl.h>

int
main()
{
    attr_list list = create_attr_list();
    attr_list list2 = create_attr_list();
    attr_list list3;
    char *string;

    add_attr(list, attr_atom_from_string("positraction"), Attr_Int4, 
	     (attr_value) 1);
    add_attr(list, attr_atom_from_string("gerald"), Attr_String, 
	     (attr_value) "whiskey");
    add_attr(list, attr_atom_from_string("domino"), Attr_Atom, 
	     (attr_value) attr_atom_from_string("pizza"));
    dump_attr_list(list);

    string = attr_list_to_string(list);
    printf("\n");
    printf("String is >%s<\n", string);
    add_attr(list2, attr_atom_from_string("sublists?"), Attr_List, list);
    list3 = attr_list_from_string(string);
    printf("\n");
    dump_attr_list(list3);
    printf("\n");
    dump_attr_list(list2);
    string = attr_list_to_string(list2);
    printf("\n");
    printf("String is >%s<\n", string);
    list3 = attr_list_from_string(string);
    printf("\n");
    dump_attr_list(list3);
    return 0;
}
