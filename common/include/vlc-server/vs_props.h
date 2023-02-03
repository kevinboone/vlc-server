/*============================================================================

  boilerplate 
  vs_props.h
  Copyright (c)2000-2023 Kevin Boone, GPL v3.0

============================================================================*/

#pragma once

#include <vlc-server/vs_defs.h> 

struct _VSProps;
typedef struct _VSProps VSProps;

VSProps    *vs_props_create (void);
void        vs_props_destroy (VSProps *self);
void        vs_props_remove_key (VSProps *self, const char *name);
void        vs_props_add (VSProps *self, const char *name, const char *value);
const char *vs_props_get (const VSProps *self, const char *name);
void        vs_props_dump (const VSProps *self);


