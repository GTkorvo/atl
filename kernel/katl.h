
/* Required definitions. */
#ifndef MODULE
#define MODULE
#endif
#ifndef __KERNEL__
#define __KERNEL__
#endif

#define katl_h 1

/* Various required header files. */
#include <linux/kernel.h>
#include <linux/kbd_ll.h>
#include <linux/types.h> 
#include <linux/utsname.h>
#include <linux/unistd.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <asm/segment.h> 
#include <asm/signal.h> 
#include <asm/ptrace.h> 

extern void initialize_artemis(void);

struct sel_arg_struct {
  unsigned long n;
  __kernel_fd_set *inp, *outp, *exp;
  struct timeval *tvp;
};

/* List of system calls used in the modules. */
extern int     (*sys_exit)       (int error_code);
extern int     (*sys_fork)       (struct pt_regs regs);
extern __kernel_ssize_t (*sys_read)  (unsigned int fd, char *buf, size_t count);
extern __kernel_ssize_t (*sys_write) (unsigned int fd, const char *buf, 
                                  size_t count);
extern int     (*sys_kopen)      (const char *filename, int flags, int mode);
extern int     (*sys_kclose)     (unsigned int fd);
extern int     (*sys_waitpid)    (__kernel_pid_t pid, 
                                  unsigned int *stat_addr, int options);
extern int     (*sys_unlink)     (const char *pathname);
extern int     (*sys_time)       (int *tloc);
extern int     (*sys_ni_syscall) (void);
extern int     (*sys_getpid)     (void);
extern int     (*sys_getuid)     (void);
extern unsigned int (*sys_alarm) (unsigned int seconds);
extern int     (*sys_kill)       (int pid, int sig);
extern int     (*sys_pipe)       (unsigned long *fildes);
extern unsigned long (*sys_signal) (int sig, __sighandler_t handler);
extern int     (*sys_sigaction)  (int sig, const struct old_sigaction *act,
                           struct old_sigaction *oact);
extern int     (*sys_setrlimit)  (unsigned int resource, struct rlimit *rlim);
extern int     (*sys_select)     (int n, fd_set *inp, 
                                  fd_set *outp,
                                  fd_set *exp, struct timeval *tvp);
extern int     (*sys_truncate)   (const char *path, unsigned long length);
extern __kernel_ssize_t (*sys_readv) (unsigned long fd, 
                                      const struct iovec *vector,
                                      unsigned long count);
extern __kernel_ssize_t (*sys_writev) (unsigned long fd, 
                         const struct iovec *vector,
                          unsigned long count);
extern int     (*sys_rt_sigaction) (int sig, const struct sigaction *act, struct
                             sigaction *oact, size_t sigsetsize);

extern int     select     (int n, fd_set *inp, 
                                  fd_set *outp,
                                  fd_set *exp, struct timeval *tvp);

/* Stdlib functions. */
extern char *strdup(char *s);
extern int   strtol(const char *nptr, char **endptr, int base);
extern int   atoi(const char *nptr);

extern struct file * my_fcheck(unsigned int fd);

typedef struct frec *frec_p;
typedef char *addrs_t;
typedef void *any_t;
typedef void *thr_mutex_t;

typedef struct frec {
  addrs_t fbp;			
  size_t size;
  frec_p next;
} frec_t;		

/* Global variables. */
extern frec_p *frhead;
extern frec_p *frecs;
extern frec_p *orhead;
extern addrs_t p_memptr;

/* Prototypes. */
extern addrs_t DKmallocMM (addrs_t p_memptr, size_t size, int priority);
extern addrs_t DAllocMM (size_t size);
extern addrs_t DAllocMM2 (size_t size);
extern void DKfreeMM (addrs_t p_memptr);
extern void DMergeRecords (frec_p frp);
extern void DDelRecord (frec_p prev_frp, frec_p frp);
extern void DInitMM (addrs_t p_memptr, size_t size);
extern addrs_t DReallocMM (addrs_t oldmem, size_t bytes);
extern int DInsertEntry (addrs_t addr, size_t size);
extern int DFindEntry (addrs_t addr);
extern void DRemoveEntry (addrs_t addr);
extern void DFreeMM (addrs_t addr);


