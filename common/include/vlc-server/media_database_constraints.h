/*============================================================================

  vlc-server 
  server/src/media_database.h
  Copyright (c)2022 Kevin Boone, GPL v3.0

============================================================================*/

#pragma once

#include <vlc-server/vs_defs.h> 
#include <vlc-server/vs_list.h> 

typedef struct _MediaDatabaseConstraints
  {
  int start;
  int count;
  char *where;
  } MediaDatabaseConstraints;

BEGIN_CDECLS

extern MediaDatabaseConstraints *media_database_constraints_new (void);

extern void media_database_constraints_destroy 
    (MediaDatabaseConstraints *self);

extern void media_database_constraints_set_where (MediaDatabaseConstraints *self, 
       const char *where);

END_CDECLS




