/*======================================================================
  
  vlc-server

  vs_storage.h

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/

#include <vlc-server/vs_defs.h>
#include <vlc-server/vs_metadata.h>

/*======================================================================
  
  structures

======================================================================*/

struct _VSStorage;
typedef struct _VSStorage VSStorage;

BEGIN_CDECLS

extern VSStorage *vs_storage_new (int capacity_mb, int free_mb,
             int albums, int tracks);
extern void vs_storage_destroy (VSStorage *self);

extern int vs_storage_get_capacity_mb (const VSStorage *self);
extern int vs_storage_get_free_mb (const VSStorage *self);
extern int vs_storage_get_albums (const VSStorage *self);
extern int vs_storage_get_tracks (const VSStorage *self);


END_CDECLS


