#ifdef MODULE
#ifndef __KERNEL__
#define __KERNEL__
#endif
#endif

#ifdef ATL_H
#define MAX_STR 256

typedef struct _syscall_attrs_atl {
    char name[MAX_STR];
    int name_len;
    char str[MAX_STR];
    int str_len;
    attr_list list;
    attr_list list1;
    attr_list list2;
    atom_t attr_id;
    attr_value_type value_type;
    attr_value value;
    attr_value_type *value_type_p;
    attr_value *value_p;
    int retval;
    int index;
    attr_p attr_p1;
    attr_p attr_p2;
    attr_p ret_attr_p;
    xmit_object ret_obj;
    xmit_attr_ref attr_ref;
    int xmit_attr_count;
    xmit_attr *xmit_list;
    atom_t ret_atom;
    atom_t atom;
    char *ret_str;
}syscall_attrs_atl, *syscall_attrs_atl_ptr;
#endif

#define printf         printk
#define perror         printk
#define fprintf(fmt, args...) printk (args) 
#define malloc         (void *)DAllocMM
#define free(a)        DFreeMM((addrs_t)a)
#define realloc(a,b)   (void *)DReallocMM((addrs_t)a,b)
#define exit           sys_exit
#define qsort          _quicksort
#define getpid()       current->pid
#define signal(a,b)    
#define gethostname    sys_gethostname
#define gethostbyname  lookup_name
#define select         sys_select
#define thr_mutex_lock(a)
#define thr_mutex_unlock(b)
#define thr_mutex_alloc() NULL
#define gen_thr_initialized() 0
