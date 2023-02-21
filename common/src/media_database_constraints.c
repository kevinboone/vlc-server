/*======================================================================
  
  vlc-server

  server/src/media_database.c

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
#include <vlc-server/media_database_constraints.h>

/*======================================================================
  media_database_constraints_new
======================================================================*/
MediaDatabaseConstraints *media_database_constraints_new (void)
  {
  MediaDatabaseConstraints *self = malloc 
     (sizeof (MediaDatabaseConstraints));
  memset (self, 0, sizeof (MediaDatabaseConstraints));
  return self;
  }

/*======================================================================
  media_database_constraints_destroy
======================================================================*/
void media_database_constraints_destroy 
    (MediaDatabaseConstraints *self)
  {
  if (self->where) free (self->where);
  free (self);
  }

/*======================================================================
  media_database_constraints_destroy
======================================================================*/
void media_database_constraints_set_where (MediaDatabaseConstraints *self, 
       const char *where)
  {
  if (self->where) free (self->where);
  self->where = strdup (where);
  }

