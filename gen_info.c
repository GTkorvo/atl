#include <stdlib.h>
#include <string.h>
#include "gen_thread.h"

gen_thread_info_t *gen_thread_info = NULL;

/*
 *  			Why is this weird?
 *	    -or- Shared libraries make strange bedfellows.
 *  
 *  gen_threads exists to provide a mechanism for libraries to have 
 *  access to thread features without compiled-in assumptions about 
 *  what thread system wil be used in the final application.  It used to
 *  work by having global data structure that was built into each protected
 *  library as a separate object file.  If a thread library was initialized,
 *  the structure was filled in with the appropriate pointers and all 
 *  libraries saw the changes.  The functionality of the linker on various
 *  platforms (Unix and WinNT) guaranteed that there would be only one copy
 *  of the data structure in the final executable and there would be no
 *  "multiply defined symbols" type of errors.
 *    Shared libraries break this simple plan.  When shared libraries are 
 *  introduced, each library may have and see only it's own copy of the
 *  "global" datastructure.  If it is initialized by the main program, the
 *  libraries won't know about it.  So, the subroutine below implements a 
 *  more complex plan.  Consider that in the shared library scenario, there
 *  are potentially multiple instances of gen_threads in the address space.
 *  The first time any function of an instance gets invoked,
 *  libgenthreads_init() is called first.  It must find out if another
 *  instance has been initialized before it, and/or take steps so that
 *  subsequent instance initializations know about this one.  We use Unix
 *  environment variables to accomplish this communication.  The first
 *  instance creates a data block and puts its address into the environment
 *  variable "GEN_INFO_ADDRESS_<pid>".  Subsequent instances just grab the
 *  address from this variable and use the already-created block.  (Yes,
 *  there is a potential race condition here, but I don't know an easy way
 *  around it and this stuff *should* be happing before we do threads calls
 *  anyway.)  Why do we have <pid> in the variable name?  Because
 *  environment variables are passed to any subprocess we fork and we those
 *  subprocs should do their own initialization.    - GSE 10/26/99
 */
int
libgenthreads_init()
{
    char *addr_str;
    char var_str[60];

    sprintf(var_str, "GEN_INFO_ADDRESS_%lx", (long) getpid());
    if ((addr_str = getenv(var_str)) == NULL) {
	char addr_tmp[64];
	gen_thread_info = malloc(sizeof(gen_thread_info_t));

	gen_thread_info->start_func = (thr_start_func_t) NULL;
	gen_thread_info->fork_func = (thr_fork_func_t) NULL;
	gen_thread_info->detach_func = (thr_detach_func_t) NULL;
	gen_thread_info->yield_func = (thr_yield_func_t) NULL;
	gen_thread_info->mutex_alloc_func = (thr_mutex_alloc_func_t) NULL;
	gen_thread_info->mutex_free_func = (thr_mutex_func_t) NULL;
	gen_thread_info->mutex_lock_func = (thr_mutex_func_t) NULL;
	gen_thread_info->mutex_unlock_func = (thr_mutex_func_t) NULL;
	gen_thread_info->condition_alloc_func = (thr_condition_alloc_func_t) NULL;
	gen_thread_info->condition_free_func = (thr_condition_func_t) NULL;
	gen_thread_info->condition_wait_func = (thr_condition_wait_func_t) NULL;
	gen_thread_info->condition_signal_func = (thr_condition_func_t) NULL;
	gen_thread_info->condition_broadcast_func = (thr_condition_func_t) NULL;
	gen_thread_info->exit_func = (thr_exit_func_t) NULL;
	gen_thread_info->join_func = (thr_join_func_t) NULL;
	gen_thread_info->self_func = (thr_self_func_t) NULL;
	gen_thread_info->null_thread_init = 0;
	gen_thread_info->is_kernel = 0;

	sprintf(addr_tmp, "%s=%lx", var_str, gen_thread_info);
	addr_str = strdup(addr_tmp);
	putenv(addr_str);
    } else {
	sscanf(addr_str, "%lx", &gen_thread_info);
	
    }
    return 1;
}


