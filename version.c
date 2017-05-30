
#include <stdio.h>
#include "config.h"

static char *ATL_version = "ATL Version 2.1.3 rev. 26897  -- 2017-05-29 16:11:06 -0400 (Mon, 29 May 2017)\n";

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

