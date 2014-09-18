#if defined(FUNCPROTO) || defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
#ifndef ARGS
#define ARGS(args) args
#endif
#else
#ifndef ARGS
#define ARGS(args) (/*args*/)
#endif
#endif

#include <stdio.h>
#include "config.h"

static char *ATL_version = "ATL Version 2.0.10 rev. 18546  -- 2014-09-17 15:00:12 -0400 (Wed, 17 Sep 2014)\n";

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

