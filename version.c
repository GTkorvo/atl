
#include <stdio.h>
#include "config.h"

static char *ATL_version = "ATL Version 2.1.1 rev. 26688  -- 2017-04-21 16:51:47 -0400 (Fri, 21 Apr 2017)\n";

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

