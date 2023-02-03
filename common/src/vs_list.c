/*============================================================================

  vs_list.c
  Copyright (c)2000-2022 Kevin Boone, GPL v3.0

  Methods for maintaining a single-linked list.

  The list _should_ be thread safe, in that only one thread can operate
  on it at a time. However, I can't say I've tested the thread-safety
  very exhaustively. 

============================================================================*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <pthread.h>
#include <vlc-server/vs_log.h>
#include <vlc-server/vs_list.h>
#include <vlc-server/vs_string.h>

typedef struct _ListItem
  {
  struct _ListItem *next;
  void *data;
  } ListItem;

struct _VSList
  {
  pthread_mutex_t mutex;
  pthread_mutexattr_t mutex_attr;
  ListItemFreeFn free_fn; 
  ListItem *head;
  };

/*==========================================================================
vs_list_create
*==========================================================================*/
VSList *vs_list_create (ListItemFreeFn free_fn)
  {
  IN
  VSList *list = malloc (sizeof (VSList));
  memset (list, 0, sizeof (VSList));
  list->free_fn = free_fn;
  pthread_mutexattr_init (&list->mutex_attr);
  pthread_mutexattr_settype (&list->mutex_attr, PTHREAD_MUTEX_RECURSIVE);
  pthread_mutex_init (&list->mutex, &list->mutex_attr);
  OUT
  return list;
  }

/*==========================================================================
  vs_list_create_strings
 
  This is a helper function for creating a list of C strings -- not
  string objects
*==========================================================================*/
VSList *vs_list_create_strings (void)
  {
  return vs_list_create (free);
  }

/*==========================================================================
  vs_list_destroy
*==========================================================================*/
void vs_list_destroy (VSList *self)
  {
  IN
  if (self) 
    {
    pthread_mutex_lock (&self->mutex);
    ListItem *l = self->head;
    while (l)
      {
      if (self->free_fn)
        self->free_fn (l->data);
      ListItem *temp = l;
      l = l->next;
      free (temp);
      }

    pthread_mutex_unlock (&self->mutex);
    pthread_mutex_destroy (&self->mutex);
    pthread_mutexattr_destroy (&self->mutex_attr);
    free (self);
    }
  OUT
  }


/*==========================================================================
vs_list_prepend
Note that the caller must not modify or free the item added to the list. It
will remain on the list until free'd by the list itself, by calling
the supplied free function
*==========================================================================*/
void vs_list_prepend (VSList *self, void *item)
  {
  IN
  pthread_mutex_lock (&self->mutex);
  ListItem *i = malloc (sizeof (ListItem));
  i->data = item;
  i->next = NULL;

  if (self->head)
    {
    i->next = self->head;
    self->head = i;
    }
  else
    {
    self->head = i;
    }
  pthread_mutex_unlock (&self->mutex);
  OUT
  }


/*==========================================================================
  vs_list_append
  Note that the caller must not modify or free the item added to the list. 
  It will remain on the list until free'd by the list itself, by calling
    the supplied free function
*==========================================================================*/
void vs_list_append (VSList *self, void *item)
  {
  IN
  pthread_mutex_lock (&self->mutex);
  ListItem *i = malloc (sizeof (ListItem));
  i->data = item;
  i->next = NULL;

  if (self->head)
    {
    ListItem *l = self->head;
    while (l->next)
      l = l->next;
    l->next = i;
    }
  else
    {
    self->head = i;
    }
  pthread_mutex_unlock (&self->mutex);
  OUT
  }


/*==========================================================================
  vs_list_length
*==========================================================================*/
int vs_list_length (VSList *self)
  {
  IN
  pthread_mutex_lock (&self->mutex);
  ListItem *l = self->head;

  int i = 0;
  while (l != NULL)
    {
    l = l->next;
    i++;
    }

  pthread_mutex_unlock (&self->mutex);
  OUT
  return i;
  }

/*==========================================================================
  vs_list_get
*==========================================================================*/
void *vs_list_get (VSList *self, int index)
  {
  IN
  pthread_mutex_lock (&self->mutex);
  ListItem *l = self->head;
  int i = 0;
  while (l != NULL && i != index)
    {
    l = l->next;
    i++;
    }
  pthread_mutex_unlock (&self->mutex);
  OUT
  return l->data;
  }


/*==========================================================================
  vs_list_dump
  For debugging purposes -- will only work at all if the list contains
  C strings
*==========================================================================*/
void vs_list_dump (VSList *self)
  {
  int i, l = vs_list_length (self);
  for (i = 0; i < l; i++)
    {
    const char *s = vs_list_get (self, i);
    printf ("%s\n", s);
    }
  }


/*==========================================================================
  vs_list_contains
*==========================================================================*/
BOOL vs_list_contains (VSList *self, const void *item, ListCompareFn fn)
  {
  IN
  pthread_mutex_lock (&self->mutex);
  ListItem *l = self->head;
  BOOL found = FALSE;
  while (l != NULL && !found)
    {
    if (fn (l->data, item, NULL) == 0) found = TRUE; 
    l = l->next;
    }
  pthread_mutex_unlock (&self->mutex);
  OUT
  return found; 
  }


/*==========================================================================
vs_list_contains_string
*==========================================================================*/
BOOL vs_list_contains_string (VSList *self, const char *item)
  {
  return vs_list_contains (self, item, (ListCompareFn)strcmp);
  }


/*==========================================================================
vs_list_remove_object
Remove the specific item from the list, if it is present. The object's
remove function will be called. This method can't be used to remove an
object by value -- that is, you can't pass "dog" to the method to remove
all strings whose value is "dog". Use vs_list_remove() for that.
*==========================================================================*/
void vs_list_remove_object (VSList *self, const void *item)
  {
  IN
  pthread_mutex_lock (&self->mutex);
  ListItem *l = self->head;
  ListItem *last_good = NULL;
  while (l != NULL)
    {
    if (l->data == item)
      {
      if (l == self->head)
        {
        self->head = l->next; // l-> next might be null
        }
      else
        {
        if (last_good) last_good->next = l->next;
        }
      self->free_fn (l->data);  
      ListItem *temp = l->next;
      free (l);
      l = temp;
      } 
    else
      {
      last_good = l;
      l = l->next;
      }
    }
  pthread_mutex_unlock (&self->mutex);
  OUT
  }


/*==========================================================================
vs_list_remove
Remove all items from the last that are a match for 'item', as
determined by a comparison function.

IMPORTANT -- The "item" argument cannot be a direct reference to an
item already in the list. If that item is removed from the list its
memory will be freed. The "item" argument will thus be an invalid
memory reference, and the program will crash when it is next used. 
It is necessary to provide a comparison function, and items will be
removed (and freed) that satisfy the comparison function. 

To remove one specific, known, item from the list, usevs_list_remove_object()
*==========================================================================*/
void vs_list_remove (VSList *self, const void *item, ListCompareFn fn)
  {
  IN
  pthread_mutex_lock (&self->mutex);
  ListItem *l = self->head;
  ListItem *last_good = NULL;
  while (l != NULL)
    {
    if (fn (l->data, item, NULL) == 0)
      {
      if (l == self->head)
        {
        self->head = l->next; // l-> next might be null
        }
      else
        {
        if (last_good) last_good->next = l->next;
        }
      self->free_fn (l->data);  
      ListItem *temp = l->next;
      free (l);
      l = temp;
      } 
    else
      {
      last_good = l;
      l = l->next;
      }
    }
  pthread_mutex_unlock (&self->mutex);
  OUT
  }

/*==========================================================================
vs_list_remove_string
*==========================================================================*/
void vs_list_remove_string (VSList *self, const char *item)
  {
  vs_list_remove (self, item, (ListCompareFn)strcmp);
  }


/*==========================================================================
vs_list_clone
*==========================================================================*/
VSList *vs_list_clone (VSList *self, ListCopyFn copyFn)
  {
  IN
  ListItemFreeFn free_fn = self->free_fn; 
  VSList *new = vs_list_create (free_fn);

  pthread_mutex_lock (&self->mutex);
  ListItem *l = self->head;
  while (l != NULL)
    {
    void *data = copyFn (l->data);
    vs_list_append (new, data);
    l = l->next;
    }
  pthread_mutex_unlock (&self->mutex);

  OUT
  return new;
  }


/*==========================================================================
  vs_list_sort

  Sort the list according to the supplied list sort function. This should
  return -1, 0, or 1 in the usual way. The arguments to this function are
  pointers to pointers to objects supplied by vs_list_append, etc., not direct
  pointers.

  The implementation is rather ugly -- we copy the list data pointers to a
  flat array, then use qsort() on the array. Then we copy the list data
  pointers back into the list. So the original chain of links remains
  unchanged, but each link is now associated with a new data item.
*==========================================================================*/
void vs_list_sort (VSList *self, ListSortFn fn, void *user_data)
  {
  IN
  pthread_mutex_lock (&self->mutex);
  int length = vs_list_length (self);
  
  void **temp = malloc (length * sizeof (void *));
  ListItem *l = self->head;
  int i = 0;
  while (l != NULL)
    {
    temp[i] = l->data;
    l = l->next;
    i++;
    }

  // Sort temp

  qsort_r (temp, length, sizeof (void *), fn, user_data); 
  
  // Copy the sorted data back
   
  l = self->head;
  i = 0;
  while (l != NULL)
    {
    l->data = temp[i];
    l = l->next;
    i++;
    }
  
  free (temp);

  pthread_mutex_unlock (&self->mutex);
  OUT
  }

