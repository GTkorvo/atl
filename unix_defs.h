#include <sys/types.h>
#if defined(FUNCPROTO) || defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
#ifndef ARGS
#define ARGS(args) args
#endif
#ifndef	NULL
#define NULL	((void *) 0)
#endif
#else
#ifndef ARGS
#define ARGS(args) (/*args*/)
#endif
#ifndef	NULL
#define NULL	0
#endif
#endif
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif

extern pid_t getpid();
extern char *getlogin();
