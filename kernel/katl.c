
/*
 *   katl.c
 *
 *   Copyright (C) 2000 Christian Poellabauer
 *
 */

/* 
 *   Kernel-loadable module for event channel support
 */

/* Required definitions. */
#ifndef MODULE
#define MODULE
#endif
#ifndef __KERNEL__
#define __KERNEL__
#endif

/* Various required header files. */
#include <linux/module.h> 
#include <linux/kernel.h> 
#include <linux/tty.h>
#include <linux/unistd.h>
#include <linux/socket.h>
#include <asm/current.h>
#include <asm/uaccess.h>
#include "katl.h"
#include "atl.h"
#include "kernel/kernel_defs.h"

/* Prototypes. */
int init_module(void);		       
void cleanup_module(void);
int (*original_syscall_atl)();

/* arch/i386/kernel/entry.S */
extern void *sys_call_table[];
extern void *fast_sys_call_table[];

/* Global variables. */

/* Internal syscall function. */
int int_syscall_atl(data)
void *data;
{
  syscall_attrs_atl *attrs;
  char *string;
  char kstring[512];

  attrs = (syscall_attrs_atl *)data;

  if (current->artemis == NULL)
    initialize_artemis();

  if (strcmp("create_attr_list", attrs->name) == 0) {
    attrs->list = create_attr_list();
    return 0;
  }
  if (strcmp("free_attr_list", attrs->name) == 0) {
    free_attr_list(attrs->list);
    return 0;
  }
  if (strcmp("add_ref_attr_list", attrs->name) == 0) {
    add_ref_attr_list(attrs->list);
    return 0;
  }
  if (strcmp("attr_join_lists", attrs->name) == 0) {
    attrs->list = attr_join_lists(attrs->list1, attrs->list2);
    return 0;
  }
  if (strcmp("attr_add_list", attrs->name) == 0) {
    attrs->list = attr_add_list(attrs->list1, attrs->list2);
    return 0;
  }
  if (strcmp("add_attr", attrs->name) == 0) {
    attrs->retval = add_attr(attrs->list, attrs->attr_id, attrs->value_type, 
	attrs->value);
    return 0;
  }
  if (strcmp("set_attr", attrs->name) == 0) {
    attrs->retval = set_attr(attrs->list, attrs->attr_id, attrs->value_type, 
	attrs->value);
    return 0;
  }
  if (strcmp("replace_attr", attrs->name) == 0) {
    attrs->retval = replace_attr(attrs->list, attrs->attr_id, attrs->value_type,
	attrs->value);
    return 0;
  }
  if (strcmp("query_attr", attrs->name) == 0) {
    attrs->retval = query_attr(attrs->list, attrs->attr_id, attrs->value_type_p,
	attrs->value_p);
    return 0;
  }
  if (strcmp("dump_attr_list", attrs->name) == 0) {
    dump_attr_list(attrs->list);
    return 0;
  }
  if (strcmp("attr_atom_from_string", attrs->name) == 0) {
    copy_from_user(kstring, attrs->str, strlen(attrs->str)+1);
    attrs->ret_atom = attr_atom_from_string(kstring);
    return 0;
  }
  if (strcmp("attr_string_from_atom", attrs->name) == 0) {
    string = attr_string_from_atom(attrs->atom);
    copy_to_user(attrs->str, string, strlen(string)+1);
    attrs->ret_str = (char *) attrs->str;
    return 0;
  }
  if (strcmp("attr_count", attrs->name) == 0) {
    attrs->retval = attr_count(attrs->list);
    return 0;
  }
  if (strcmp("get_attr", attrs->name) == 0) {
    get_attr(attrs->list, attrs->index, &attrs->attr_id, &attrs->value_type, 
	     &attrs->value);
    return 0;
  }
  if (strcmp("attr_list_to_string", attrs->name) == 0) {
    string = attr_list_to_string(attrs->list);
    copy_to_user(attrs->str, string, strlen(string)+1);
    attrs->ret_str = (char *) attrs->str;
    return 0;
  }
  if (strcmp("attr_list_from_string", attrs->name) == 0) {
    copy_from_user(kstring, attrs->str, strlen(attrs->str)+1);
    attrs->list = attr_list_from_string(kstring);
    return 0;
  }
  if (strcmp("attr_list_subset", attrs->name) == 0) {
    attrs->retval = attr_list_subset(attrs->list1, attrs->list2);
    return 0;
  }
  else
    return -1;
}

/* arch/i386/kernel/entry.S */
extern void *sys_call_table[];

/* Initialize module. */
int init_module(void) 
{
  original_syscall_atl = sys_call_table[__NR_KECho_syscall_atl];

  sys_call_table[__NR_KECho_syscall_atl] = int_syscall_atl;

  return 0;
}

/* Remove module. */
void cleanup_module(void) 
{
  sys_call_table[__NR_KECho_syscall_atl] = original_syscall_atl;
}

