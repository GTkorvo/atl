
#include <config.h>
#include <stdlib.h>
#include <stdio.h>
#include <unix_defs.h>
#include <atom.h>

int
main()
{
    atom_server as = init_atom_server(prefill_atom_cache);
    printf("Connected to server \"%s\"\n", get_server_id(as));
    while (1) {
	char line[80];
	atom_t atom;

	fgets(stdin, line, sizeof(line));
	if (sscanf(line, "%d", &atom) == 1) {
	    char *str;
	    printf("inquiring for atom %d\n", atom);
	    str = string_from_atom(as, atom);
	    if (str) {
		printf("	string translation is \"%s\"\n", str);
		free(str);
	    } else {
		printf("	no atom %d\n", atom);
	    }
	} else {
	    printf("inquiring for string \"%s\"\n", line);
	    printf("	atom value is %d\n", atom_from_string(as, line));
	}
    }
}
