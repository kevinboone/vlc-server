/*======================================================================
  
  vlc-server

  vs_search_constraints.c 

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <regex.h>
#include <microhttpd.h>
#include <vlc-server/vs_log.h> 
#include <vlc-server/vs_util.h> 
#include <vlc-server/vs_string.h> 
#include <vlc-server/vs_search_constraints.h>

/*======================================================================
  vs_search_constraints_new
======================================================================*/
VSSearchConstraints *vs_search_constraints_new (void)
  {
  VSSearchConstraints *self = malloc 
     (sizeof (VSSearchConstraints));
  memset (self, 0, sizeof (VSSearchConstraints));
  return self;
  }

/*======================================================================
  vs_search_constraints_destroy
======================================================================*/
void vs_search_constraints_destroy 
    (VSSearchConstraints *self)
  {
  if (self->where) free (self->where);
  free (self);
  }

/*======================================================================
  vs_search_constraints_destroy
======================================================================*/
void vs_search_constraints_set_where (VSSearchConstraints *self, 
       const char *where)
  {
  if (self->where) free (self->where);
  self->where = strdup (where);
  }

