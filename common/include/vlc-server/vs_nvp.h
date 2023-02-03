/*============================================================================

  boilerplate 
  vs_nvp.h
  Copyright (c)2000-2023 Kevin Boone, GPL v3.0

============================================================================*/

#pragma once

#include <vlc-server/vs_defs.h> 

struct _VSNVP;
typedef struct _VSNVP VSNVP;

VSNVP      *vs_nvp_create (const char *key, const char *value);
void        vs_nvp_destroy (VSNVP *self);
const char *vs_nvp_get_name (const VSNVP *self);
const char *vs_nvp_get_value (const VSNVP *self);

