
#include <stdio.h>
#include "config.h"

static char *ATL_version = "ATL Version 2.1.0 rev. 25895  -- 2016-10-13 08:22:44 -0400 (Thu, 13 Oct 2016)\n";

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

