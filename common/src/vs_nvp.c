/*============================================================================

  vs_nvp.c
  Copyright (c)2000-2022 Kevin Boone, GPL v3.0

  Methods for storing a name-value pair.

============================================================================*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <pthread.h>
#include <vlc-server/vs_log.h>
#include <vlc-server/vs_string.h>
#include <vlc-server/vs_nvp.h>

struct _VSNVP
  {
  char *name;
  char *value;
  };

/*==========================================================================
vs_nvp_create
*==========================================================================*/
VSNVP *vs_nvp_create (const char *name, const char *value)
  {
  IN
  VSNVP *self = malloc (sizeof (VSNVP));
  self->name = strdup (name);
  self->value = strdup (value ? value : "");
  OUT
  return self;
  }

/*==========================================================================
  vs_nvp_destroy
*==========================================================================*/
void vs_nvp_destroy (VSNVP *self)
  {
  IN
  if (self) 
    {
    free (self->name);
    free (self->value);
    free (self);
    }
  OUT
  }

/*==========================================================================
  vs_nvp_get_name
*==========================================================================*/
const char *vs_nvp_get_name (const VSNVP *self)
  {
  return self->name;
  }

/*==========================================================================
  vs_nvp_get_value
*==========================================================================*/
const char *vs_nvp_get_value (const VSNVP *self)
  {
  return self->value;
  }



