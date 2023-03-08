/*============================================================================

  vlc-server 

  server/src/vs_search_constraints.h

  Copyright (c)2023 Kevin Boone, GPL v3.0

============================================================================*/

#pragma once

#include <vlc-server/vs_defs.h> 
#include <vlc-server/vs_list.h> 

typedef struct _VSSearchConstraints
  {
  int start;
  int count;
  char *where;
  } VSSearchConstraints;

BEGIN_CDECLS

extern VSSearchConstraints *vs_search_constraints_new (void);

extern void vs_search_constraints_destroy 
    (VSSearchConstraints *self);

extern void vs_search_constraints_set_where (VSSearchConstraints *self, 
       const char *where);

END_CDECLS




