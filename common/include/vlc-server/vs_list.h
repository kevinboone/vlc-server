/*============================================================================

  boilerplate 
  vs_list.h
  Copyright (c)2000-2020 Kevin Boone, GPL v3.0

============================================================================*/

#pragma once

#include <vlc-server/vs_defs.h> 

struct _VSList;
typedef struct _VSList VSList;

// The comparison function should return -1, 0, +1, like strcmp. In practice
//   however, the functions that use this only care whether too things 
//   are equal -- ordering is not important. The i1,i2 arguments are 
//   pointers to the actual objects in the list. user_data is not used
//   at present
typedef int (*ListCompareFn) (const void *i1, const void *i2, 
          void *user_data);

// A comparison function for vs_list_sort. Rather confusingly, it looks exactly
//   like ListCompareFn, but the argument type is different. Here the i1,i2
//   are the addresses of pointers to objects in the list, not pointers.
//   So they are pointers to pointers. Sorry, but this is the way the
//   underlying qsort implementation works. For an example of coding a
//   sort function, see string_alpha_sort_fn. The user_data argument is
//   the value passed to the vs_list_sort function itself, and is relevant
//   only to the caller
typedef int (*ListSortFn) (const void *i1, const void *i2, 
          void *user_data);

typedef void* (*ListCopyFn) (const void *orig);
typedef void (*ListItemFreeFn) (void *);

VSList *vs_list_create (ListItemFreeFn free_fn);
void    vs_list_destroy (VSList *self);
void    vs_list_append (VSList *self, void *item);
void    vs_list_prepend (VSList *self, void *item);
void   *vs_list_get (VSList *self, int index);
void    vs_list_dump (VSList *self);
int     vs_list_length (VSList *self);
BOOL    vs_list_contains (VSList *self, const void *item, ListCompareFn fn);
BOOL    vs_list_contains_string (VSList *self, const char *item);
void    vs_list_remove (VSList *self, const void *item, ListCompareFn fn);
void    vs_list_remove_string (VSList *self, const char *item);
VSList *vs_list_clone (VSList *self, ListCopyFn copyFn);
VSList *vs_list_create_strings (void);
void    vs_list_remove_object (VSList *self, const void *item);
void    vs_list_sort (VSList *self, ListSortFn fn, void *user_data);

