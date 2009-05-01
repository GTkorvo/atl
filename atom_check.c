
#include <config.h>
#include <stdlib.h>
#include <stdio.h>
#include <unix_defs.h>
#include <atl.h>
#include <ctype.h>
#include <string.h>

int
main()
{
    atom_t atom = attr_atom_from_string("ECho_attr_test_atom");
    printf("Atom test for ECho_Attr_test_atom is %x\n", atom);
    return 0;
}
