#include "gen_thread.h"

gen_thread_info_t gen_thread_info = {
    (thr_start_func_t) NULL /* start_func */,
    (thr_fork_func_t) NULL /* fork_func */,
    (thr_detach_func_t) NULL /* detach_func */,
    (thr_yield_func_t) NULL /* yield_func */,
    (thr_mutex_alloc_func_t) NULL /* mutex_alloc_func */,
    (thr_mutex_func_t) NULL /* mutex_free_func */,
    (thr_mutex_func_t) NULL /* mutex_lock_func */,
    (thr_mutex_func_t) NULL /* mutex_unlock_func */,
    (thr_condition_alloc_func_t) NULL /* condition_alloc_func */,
    (thr_condition_func_t) NULL /* condition_free_func */,
    (thr_condition_wait_func_t) NULL /* condition_wait_func */,
    (thr_condition_func_t) NULL /* condition_signal_func */,
    (thr_condition_func_t) NULL /* condition_broadcast_func */,
    (thr_exit_func_t) NULL /* exit_func */,
    (thr_join_func_t) NULL /* join_func */,
    (thr_self_func_t) NULL /* self_func */,
    (int) 0		   /* NULL threads initialized */,
    (int) 0		   /* is_kernel level threads system */
};

