
#include <config.h>
#include <stdlib.h>
#include <stdio.h>
#include <unix_defs.h>
#include <atl.h>

int
main()
{
static int k = 1000;
    atom_server as = init_atom_server(prefill_atom_cache);
    printf("Connected to server \"%s\"\n", get_server_id(as));
    while (1) {
	char line[80], buf[80];
        int atom = 0;
        static int set_atom = 4567;
	int len, test;

	if (fgets(line, sizeof(line), stdin) == NULL) return 0;
	len = strlen(line);
	if (len > 0) line[len -1] = 0;  /* truncate \n */
//  Added by Bala - ignore empty string
	   if(line[0]==0) continue;
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
	   char *str;
	   sscanf(line, "%s", &buf);
	   str = (char *)malloc(sizeof(char)*6); 
	  if(buf[0] != 'a'){
		sprintf(str, "%d", set_string_and_atom(as, buf, k));
		if(str[0] == '0') printf("Invalid string\n");
		else printf("Atom value is %s\n", str); 
	  } else {
printf("Enter the string\n");
scanf("%s", &buf);
  		sprintf(str, "%d", set_string_and_atom(as, buf, set_atom));
 		set_atom++;
		if(str[0] == '0') printf("Atom|string value already present\n");
		else printf("Atom value is %s\n", str); 
          }
           free(str);
	}
    }
}
