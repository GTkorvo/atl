#include <atom.h>
#include <attr.h>

void
main()
{
    attr_list list = create_attr_list();
    attr_list list2 = create_attr_list();
    attr_list list3 = create_attr_list();

    add_attr(list, attr_atom_from_string("positraction"), Attr_Int4, 1);
    add_attr(list, attr_atom_from_string("gerald"), Attr_String, "whiskey");
    add_attr(list, attr_atom_from_string("domino"), Attr_Atom, attr_atom_from_string("pizza"));
    dump_attr_list(list);

    add_attr(list2, attr_atom_from_string("sublists?"), Attr_List, list);
    dump_attr_list(list2);
}
