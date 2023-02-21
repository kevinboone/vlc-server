/*============================================================================

  vs_props.c
  Copyright (c)2000-2022 Kevin Boone, GPL v3.0

  Methods for storing a set of name-value pairs.

============================================================================*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <pthread.h>
#include <vlc-server/vs_log.h>
#include <vlc-server/vs_list.h>
#include <vlc-server/vs_nvp.h>
#include <vlc-server/vs_props.h>

struct _VSProps
  {
  VSList *list;
  };

/*==========================================================================
vs_props_create
*==========================================================================*/
VSProps *vs_props_create (void)
  {
  IN
  VSProps *self = malloc (sizeof (VSProps));
  self->list = vs_list_create ((ListItemFreeFn)vs_nvp_destroy);
  OUT
  return self;
  }

/*==========================================================================
  vs_props_destroy
*==========================================================================*/
void vs_props_destroy (VSProps *self)
  {
  IN
  if (self) 
    {
    if (self->list) vs_list_destroy (self->list);
    free (self);
    }
  OUT
  }

/*==========================================================================
  vs_props_remove_key
*==========================================================================*/
static int vs_props_remove_compare (const void *item, const void *name,
         void *user_data)
  {
  VSNVP *candidate = (VSNVP *)item;
  const char *cand_name = vs_nvp_get_name (candidate);
  return strcmp ((const char *)name, cand_name);
  }

/*==========================================================================
  vs_props_remove_key
*==========================================================================*/
void vs_props_remove_key (VSProps *self, const char *name)
  {
  IN
  vs_list_remove (self->list, name, vs_props_remove_compare);
  OUT
  }

/*==========================================================================
  vs_props_add
*==========================================================================*/
void vs_props_add (VSProps *self, const char *name, const char *value)
  {
  IN
  vs_props_remove_key (self, name);
  VSNVP *nvp = vs_nvp_create (name, value);
  vs_list_append (self->list, nvp);
  OUT
  }

/*==========================================================================
  vs_props_get
*==========================================================================*/
const char *vs_props_get (const VSProps *self, const char *name)
  {
  IN
  const char *ret = NULL;

  int l = vs_list_length (self->list);
  for (int i = 0; (i < l) && !ret; i++)
    {
    const VSNVP *cand_nvp = vs_list_get (self->list, i);
    const char *cand_name = vs_nvp_get_name (cand_nvp);
    if (strcmp (cand_name, name) == 0)
      {
      ret = vs_nvp_get_value (cand_nvp);
      }
    }

  OUT
  return ret;
  }

/*==========================================================================
  vs_props_dump
*==========================================================================*/
void vs_props_dump (const VSProps *self)
  {
  IN
  int l = vs_list_length (self->list);
  for (int i = 0; i < l; i++)
    {
    const VSNVP *nvp = vs_list_get (self->list, i);
    const char *name = vs_nvp_get_name (nvp);
    const char *value = vs_nvp_get_value (nvp);
    printf ("name=%s value=%s\n", name, value);
    }

  OUT
  }




