/*============================================================================

  vlc-server 

  server/src/media_database.h

  Functions for constructing and querying the media database.

  Copyright (c)2022 Kevin Boone, GPL v3.0

============================================================================*/

#pragma once

#include <vlc-server/vs_defs.h> 
#include <vlc-server/vs_list.h> 
#include <vlc-server/vs_metadata.h> 
#include <vlc-server/libvlc_media_database.h> 
#include <vlc-server/vs_search_constraints.h> 

struct _MediaDatabase;
typedef struct _MediaDatabase MediaDatabase;

/** The type of the function that will be called on each database row
       by iterate_all_paths(). */
typedef BOOL (*DBPathIteratorCallback) (const char *path, void *data);

#define MDB_ERR_NOT_INIT "Media database not initialized"

BEGIN_CDECLS

/**** MediaDatabase methods *****/

/** Create a MediaDatabase object. This only initializes the data structure;
      it does not initialize the actual database. */
extern MediaDatabase *media_database_create (const char *mdb_file);

/** Initialize the sqlite database. Returns an error message in *error
      if it cannot be initialized. */
extern void media_database_init (MediaDatabase *self, BOOL create, 
         char **error);

/** Get the filename that was supplied with the object was constructed. */
extern const char *media_database_get_filename (const MediaDatabase *self);

/** Clear up the object, and close the sqlite database. */
extern void media_database_destroy (MediaDatabase *self);

/** Performs a search of the track list according to the constraints in 
      'constraints'. The results are added to the 'results' list. 
    Each result added to the list will be an entry from the database column
       specified by the 'column' argument. */
extern void media_database_search (MediaDatabase *self,
        MediaDatabaseColumn column, VSList *results, 
       const VSSearchConstraints *constraints,  char **error);

/** As media_database_search(), except that this method only returns the
       number of hits. */
extern int media_database_search_count (MediaDatabase *self, 
       MediaDatabaseColumn column,   
       const VSSearchConstraints *constraints, char **error);

/** Returns TRUE if the sqlite database got initialized correctly. */
extern BOOL media_database_is_init (const MediaDatabase *self);

/** Get an VSMetadata object for a specific path. If there is
      not such path, or there was an error, return NULL. The caller
      must from the metadata object if it is not NULL. */
extern VSMetadata *media_database_get_amd (MediaDatabase *self, 
          const char *path);

/** Set all the fields of an VSMetadata object that have columns in the
      database, given a specific path. */
extern BOOL media_database_set_amd (MediaDatabase *self, 
          const VSMetadata *amd, char **error);

/* Return TRUE if the path is in the database, whatever data is associated 
     with it.  If not found, or on error, returns FALSE. This function is
     called by the media scanner, to check whether each file it finds is
     already in the database. */
extern BOOL media_database_has_path (MediaDatabase *self, 
          const char *path);

/** Call the specified callback function for every path in the database. */
extern BOOL media_database_iterate_all_paths (MediaDatabase *self,
        DBPathIteratorCallback callback, void *user_data, char **error);

extern void media_database_mark_path_deleted (MediaDatabase *self, 
        const char *path);

/** When the scanner builds the media datbase, the 'exists' field is set
      to 1. When doing a rescan, if a file exists in the datbase, but
      has no corresponding file on disk it is assumed to be removed, and
      exists is set to 0. A later part of the scan removes these rows. */ 
extern void media_database_remove_nonexistent (MediaDatabase *self);

/** Escape characters like the single-quote, that need a particular
      representation in SQL. */
extern char *media_database_escape_sql (const char *sql);

/**** VSSearchConstraints methods *****/

extern VSSearchConstraints *vs_search_constraints_new (void);
extern void vs_search_constraints_destroy (VSSearchConstraints *self);

END_CDECLS


