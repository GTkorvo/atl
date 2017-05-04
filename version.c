
#include <stdio.h>
#include "config.h"

static char *ATL_version = "ATL Version 2.1.2 rev. 26800  -- 2017-05-03 15:09:47 -0400 (Wed, 03 May 2017)\n";

#if defined (__INTEL_COMPILER)
//  Allow extern declarations with no prior decl
#  pragma warning (disable: 1418)
#endif
void ATLprint_version()
{
    printf("%s",ATL_version);
}
void ATLfprint_version(FILE*out)
{
    fprintf(out, "%s",ATL_version);
}

