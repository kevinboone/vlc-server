/*============================================================================

  vlc-server 
  server/src/media_database.h
  Copyright (c)2022 Kevin Boone, GPL v3.0

============================================================================*/

#pragma once

#include <vlc-server/vs_defs.h> 
#include <vlc-server/vs_list.h> 
#include <vlc-server/audio_metadata.h> 
#include <vlc-server/libvlc_media_database.h> 
#include <vlc-server/media_database_constraints.h> 

struct _MediaDatabase;
typedef struct _MediaDatabase MediaDatabase;

typedef BOOL (*DBPathIteratorCallback) (const char *path, void *data);

#define MDB_ERR_NOT_INIT "Media database not initialized"

BEGIN_CDECLS

extern MediaDatabase *media_database_create (const char *mdb_file);
extern void media_database_init (MediaDatabase *self, BOOL create, char **error);
extern const char *media_database_get_filename (const MediaDatabase *self);
extern void media_database_destroy (MediaDatabase *self);
extern void media_database_search (MediaDatabase *self,
        MediaDatabaseColumn column, VSList *results, 
       const MediaDatabaseConstraints *constraints,  char **error);
extern int media_database_search_count (MediaDatabase *self, 
       MediaDatabaseColumn column,   
       const MediaDatabaseConstraints *constraints, char **error);

extern MediaDatabaseConstraints *media_database_constraints_new (void);
extern void media_database_constraints_destroy (MediaDatabaseConstraints *self);
extern BOOL media_database_is_init (const MediaDatabase *self);

extern AudioMetadata *media_database_get_amd (MediaDatabase *self, 
          const char *path);
extern BOOL media_database_set_amd (MediaDatabase *self, 
          const AudioMetadata *amd, char **error);

// Return TRUE if the path is in the database, whatever data is associated with it.
// If not found, or on error, returns FALSE
extern BOOL media_database_has_path (MediaDatabase *self, 
          const char *path);

extern BOOL media_database_iterate_all_paths (MediaDatabase *self,
        DBPathIteratorCallback callback, void *user_data, char **error);

extern void media_database_mark_path_deleted (MediaDatabase *self, 
        const char *path);

extern void media_database_remove_nonexistent (MediaDatabase *self);

extern char *media_database_escape_sql (const char *sql);

END_CDECLS




