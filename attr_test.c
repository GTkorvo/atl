#include <atl.h>

int
main()
{
    attr_list list = create_attr_list();
    attr_list list2 = create_attr_list();

    add_attr(list, attr_atom_from_string("positraction"), Attr_Int4, 
	     (attr_value) 1);
    add_attr(list, attr_atom_from_string("gerald"), Attr_String, 
	     (attr_value) "whiskey");
    add_attr(list, attr_atom_from_string("domino"), Attr_Atom, 
	     (attr_value) attr_atom_from_string("pizza"));
    dump_attr_list(list);

    add_attr(list2, attr_atom_from_string("sublists?"), Attr_List, list);
    dump_attr_list(list2);
    return 0;
}
