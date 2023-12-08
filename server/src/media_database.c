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
#include <time.h>
#include <pthread.h>
#include <microhttpd.h>
#include <vlc-server/vs_log.h> 
#include <vlc-server/vs_util.h> 
#include <vlc-server/vs_string.h> 
#include <vlc-server/vs_metadata.h> 
#include "media_database.h"
#include "sqlite3.h"

#define SAFE(x)((x) ? (x) : "")

typedef struct _IteratePathsCallbackData
  {
  DBPathIteratorCallback callback;
  void *user_data;
  } IteratePathsCallbackData;

struct _MediaDatabase
  {
  char *mdb_file;
  sqlite3 *sqlite;
  pthread_mutex_t mutex;
  };

#define SAFE(x)((x) ? (x) : "")

/*======================================================================
  media_database_daysold
======================================================================*/
void media_database_daysold (sqlite3_context* context, int argc,
      sqlite3_value** values)
  {
  if ( argc != 1)
    {
    sqlite3_result_error (context,
      "SQL function daysago() called with invalid arguments.\n", -1);
    return;
    }

  int arg = sqlite3_value_int (values[0]);

  int daysold = (time(NULL) - arg) / (24 * 3600);

  sqlite3_result_int (context, daysold); 
  }

/*======================================================================
  media_database_regexp
======================================================================*/
static void media_database_regexp (sqlite3_context* context, int argc,
      sqlite3_value** values)
  {
  int ret;
  MediaDatabase *self = sqlite3_user_data (context);

  static char last_regex_string[128]; // Will be initialzed all zeros
  static regex_t regex;

  // The method I'm using for caching the compiled regex will only work
  //   when there are low levels of concurrency. Only one value is cached
  //   for all requests on all threads. So if two threads are doing different
  //   queries using regex, most likely the regex will still be compiled on
  //   each threads. And we have to use mutexes, because we can't risk the
  //   compiled regex changing in the middle of a query. All in all, this
  //   is ugly, and the benefit of caching is unclear.
  if (argc == 2)
    {
    char *text = (char*)sqlite3_value_text(values[1]);
    if (text)
      {
      pthread_mutex_lock (&self->mutex);
      char *reg = (char*)sqlite3_value_text(values[0]);
      if (strcmp (reg, last_regex_string)) // This is a different regex
        {
        ret = regcomp (&regex, reg, REG_EXTENDED | REG_NOSUB | REG_ICASE);
        if (ret != 0)
	  {
	  sqlite3_result_error 
            (context, "error compiling regular expression", -1);
          pthread_mutex_unlock (&self->mutex);
	  return;
	  }
        strncpy (last_regex_string, reg, sizeof (last_regex_string) - 1);
        }
      
      ret = regcomp (&regex, reg, REG_EXTENDED | REG_NOSUB | REG_ICASE);
      if (ret != 0)
	{
	sqlite3_result_error (context, 
          "error compiling regular expression", -1);
        pthread_mutex_unlock (&self->mutex);
	return;
	}

      ret = regexec (&regex, text , 0, NULL, 0);
      regfree(&regex);

      sqlite3_result_int (context, (ret != REG_NOMATCH));
      pthread_mutex_unlock (&self->mutex);
      }
    else
      {
      // A 'null' column should not match anything
      sqlite3_result_int (context, 0);
      }
    }
  else
    {
    sqlite3_result_error (context,
      "SQL function regexp() requires two arguments", -1);
    }
  }

/*======================================================================
  media_database_create
======================================================================*/
MediaDatabase *media_database_create (const char *mdb_file)
  {
  IN
  MediaDatabase *self = malloc (sizeof (MediaDatabase));
  self->mdb_file = strdup (mdb_file);
  pthread_mutex_init (&self->mutex, NULL);
  return self;
  OUT
  }

/*======================================================================
  media_database_exec
======================================================================*/
static BOOL media_database_exec (MediaDatabase *self, 
             const char *sql, char **error)
  {
  IN
  BOOL ret = FALSE;
  char *e = NULL;
  vs_log_debug ("%s: executing SQL %s", __PRETTY_FUNCTION__, sql);
  sqlite3_exec (self->sqlite, sql, NULL, 0, &e);
  if (e)
    {
    if (error) (*error) = strdup (e);
    sqlite3_free (e);
    ret = FALSE;
    }
  else
    {
    ret = TRUE;
    }
  OUT
  return ret;
  }

/*======================================================================
  media_database_exec_log_error
======================================================================*/
static void media_database_exec_log_error (MediaDatabase *self, 
             const char *sql)
  {
  IN
  char *error = NULL;
  media_database_exec (self, sql, &error);
  if (error)
    {
    vs_log_error ("Failed to execute '%s': %s", sql, error);
    }
  OUT
  }

/*======================================================================
  media_database_create_tables
======================================================================*/
static void media_database_create_tables (MediaDatabase *self)
  {
  IN
  vs_log_info ("Creating database tables");

  media_database_exec_log_error (self, "create table files "
       "(path varchar not null primary key, size integer, mtime integer, "
       "title varchar, album varchar, genre varchar, "
       "composer varchar, artist varchar, album_artist varchar, " 
       "track varchar, comment varchar, year varchar, exist integer)");

  media_database_exec_log_error (self, 
       "create index albumindex on files (album)");
  media_database_exec_log_error (self, 
       "create index artistindex on files (artist)");
  media_database_exec_log_error (self, 
       "create index albumartistindex on files (album_artist)");
  media_database_exec_log_error (self, 
       "create index composerindex on files (composer)");
  media_database_exec_log_error (self, 
       "create index pathindex on files (path)");
  media_database_exec_log_error (self, 
       "create index genreindex on files (genre)");
  OUT
  }

/*======================================================================
  media_database_init
======================================================================*/
void media_database_init (MediaDatabase *self, BOOL create, char **error)
  {
  IN
  vs_log_debug ("Opening database %s", self->mdb_file);
  BOOL create_tables = FALSE;

  if (access (self->mdb_file, R_OK) == 0)
    {
    vs_log_info ("Media database exists");
    }
  else
    {
    // If there is no DB file at all, we will have to create tables in
    //   it if we create the database.
    create_tables = TRUE;
    }

  if ((access (self->mdb_file, W_OK) == 0) || create)
    {
    sqlite3 *sqlite = NULL;
    int err = sqlite3_open (self->mdb_file, &sqlite);
    if (err == 0)
      {
      sqlite3_create_function (sqlite, "regexp", 2, SQLITE_ANY, self,
          media_database_regexp, 0, 0);
      sqlite3_create_function (sqlite, "daysold", 1, SQLITE_ANY, self,
          media_database_daysold, 0, 0);

      self->sqlite = sqlite;

      if (create_tables)
        {
        vs_log_info ("Creating an empty media database");
        media_database_create_tables (self);
        }
      }
    else
      {
      asprintf (error, "Can't open sqlite3 database '%s'", 
         self->mdb_file);
      }
    }
  else
    {
    asprintf (error, "Can't open database file '%s' in read/write mode", 
       self->mdb_file);
    }
  OUT
  }

/*======================================================================
  media_database_destroy
======================================================================*/
void media_database_destroy (MediaDatabase *self)
  {
  IN
  if (self)
    {
    if (self->sqlite) sqlite3_close (self->sqlite);
    if (self->mdb_file) free (self->mdb_file); 
    pthread_mutex_unlock (&self->mutex);
    pthread_mutex_destroy (&self->mutex);
    free (self);
    }
  OUT
  }

/*======================================================================
  media_database_col_name
======================================================================*/
static const char *media_database_col_name (MediaDatabaseColumn col)
  {
  IN
  switch (col)
    {
    case MDB_COL_PATH: return "path";
    case MDB_COL_ALBUM: return "album";
    case MDB_COL_ARTIST: return "artist";
    case MDB_COL_COMPOSER: return "composer";
    case MDB_COL_GENRE: return "genre";
    case MDB_COL_ALBUM_ARTIST: return "album_artist";
    case MDB_COL_TITLE: return "title";
    }
  return NULL; // Can not happen
  OUT
  }

/*======================================================================
  media_database_escape_sql
======================================================================*/
char *media_database_escape_sql (const char *sql)
  {
  IN
  char *ret;
  if (strchr (sql, '\''))
    {
    int l = strlen (sql);
    int newlen = l;
    ret = malloc (newlen + 1);
    int p = 0;

    for (int i = 0; i < l; i++)
      {
      char c = sql[i];
      if (c == '\'')
        {
        newlen+=2;
        ret = realloc (ret, newlen + 1);
        ret[p++] = '\'';
        ret[p++] = '\'';
        }
      else
        {
        ret[p++] = c;
        }
      }
    ret[p] = 0;
    }
  else
    {
    ret = strdup (sql);
    }
  return ret;
  OUT
  }

/*======================================================================
  media_database_sql_query

  Returns a List of char *, which may be empty. The list returned includes
  all rows and columns in row order, all concetenated into one long list.
  If the query selects a single column, then the list returned will be
  the same length as the number of matches, or 'limit' if that is
  smaller. If limit == 0, then no limit is applied

======================================================================*/
VSList *media_database_sql_query (MediaDatabase *self, const char *sql,
            BOOL include_empty, int limit, char **error)
  {
  IN
  VSList *ret = NULL;
  char *e = NULL;
  char **result = NULL;
  int hits;
  int cols;
  vs_log_debug ("%s: executing SQL %s", __PRETTY_FUNCTION__, sql);
  sqlite3_get_table (self->sqlite, sql, &result, &hits, &cols, &e);
  if (e)
    {
    if (error) (*error) = strdup (e);
    sqlite3_free (e);
    ret = NULL;
    }
  else
    {
    ret = vs_list_create (free);

    for (int i = 0; i < hits && ((i < limit) || (limit == 0)); i++)
      {
      for (int j = 0; j < cols ; j++)
        {
        char *res = result[(i + 1) * cols + j];
        if (res)
          {
          if (res[0] != 0 || include_empty)
            {
            vs_list_append (ret, strdup (res));
            }
          }
        else
          {
          if (include_empty)
            {
            vs_list_append (ret, strdup (""));
            }
          }
        }
      }
    sqlite3_free_table (result);
    }
  OUT
  return ret;
  }

/*======================================================================
  media_database_select_random
======================================================================*/
void media_database_select_random (MediaDatabase *self, 
       MediaDatabaseColumn column,  int limit, VSList *results, 
       char **error)
  {
//SELECT * FROM table ORDER BY RANDOM() LIMIT 1;
  VSString *sql = vs_string_create ("select ");

  vs_string_append (sql, media_database_col_name (column));

  vs_string_append_printf (sql, " from files order by random() limit %d", limit);

  //printf ("Executing sql %s\n", vs_string_cstr (sql));
  vs_log_debug ("Executing sql %s\n", vs_string_cstr (sql));

  int hits = 0;
  int cols = 0;
  char **result = NULL;
  char *e = NULL;
  sqlite3_get_table (self->sqlite, vs_string_cstr (sql), &result, 
     &hits, &cols, &e);
  if (e == NULL)
    {
    vs_log_debug ("Query returned %d hits", hits);

    for (int i = 0; i < hits; i++)
      {
      char *res = result [i + 1];
      vs_list_append (results, strdup (res));
      }

    sqlite3_free_table (result);
    }
  else
    {
    *error = strdup (e);
    sqlite3_free (e);
    }

  vs_string_destroy (sql);
  }

/*======================================================================
  media_database_search
======================================================================*/
void media_database_search (MediaDatabase *self, 
       MediaDatabaseColumn column,  VSList *results, 
       const VSSearchConstraints *constraints, char **error)
  {
  VSString *sql = vs_string_create ("select distinct ");

  vs_string_append (sql, media_database_col_name (column));

  vs_string_append (sql, " from files ");

  if (constraints->where)
    {
    vs_string_append (sql, " where ");
    //char *escaped_where = media_database_escape_sql (constraints->where);
    //vs_string_append (sql, escaped_where);
    //printf ("constaints_where=%s\n", constraints->where);
    vs_string_append (sql, constraints->where);
    //free (escaped_where);
    }

  vs_string_append (sql, " order by ");

  if (column == MDB_COL_PATH)
    vs_string_append (sql, "track,path"); 
  else
    vs_string_append (sql, media_database_col_name (column)); 

  //printf ("Executing sql %s\n", vs_string_cstr (sql));
  vs_log_debug ("Executing sql %s\n", vs_string_cstr (sql));

  int hits = 0;
  int cols = 0;
  char **result = NULL;
  char *e = NULL;
  sqlite3_get_table (self->sqlite, vs_string_cstr (sql), &result, 
     &hits, &cols, &e);
  if (e == NULL)
    {
    vs_log_debug ("Query returned %d hits", hits);

    for (int i = 0; i < hits; i++)
      {
      if ((i >= constraints->start) && 
                 ((i < constraints->start + constraints->count) || 
                 (constraints->count < 0)))
        {
        char *res = result [i + 1];
        vs_list_append (results, strdup (res));
        }
      }

    sqlite3_free_table (result);
    }
  else
    {
    *error = strdup (e);
    sqlite3_free (e);
    }

  vs_string_destroy (sql);
  }

/*======================================================================
  media_database_search_count
======================================================================*/
int media_database_search_count (const MediaDatabase *self, 
       MediaDatabaseColumn column,   
       const VSSearchConstraints *constraints, char **error)
  {
  int ret = 0;
  VSString *sql = vs_string_create ("select count (distinct ");

  vs_string_append (sql, media_database_col_name (column));

  vs_string_append (sql, ") from files ");

  if (constraints && constraints->where)
    {
    vs_string_append (sql, " where ");
    //char *escaped_where = media_database_escape_sql (constraints->where);
    //vs_string_append (sql, escaped_where);
    vs_string_append (sql, constraints->where);
    //free (escaped_where);
    }

  //printf ("Executing sql %s\n", vs_string_cstr (sql));
  vs_log_debug ("Executing sql %s\n", vs_string_cstr (sql));

  int hits = 0;
  int cols = 0;
  char **result = NULL;
  char *e = NULL;
  sqlite3_get_table (self->sqlite, vs_string_cstr (sql), &result, 
     &hits, &cols, &e);
  if (e == NULL)
    {
    vs_log_debug ("Query returned %d hits", hits);

    if (hits >= 1)
      {
      char *res = result [1];
      ret = atoi (res);
      }

    sqlite3_free_table (result);
    }
  else
    {
    *error = strdup (e);
    sqlite3_free (e);
    }

  vs_string_destroy (sql);
  return ret;
  }

/*======================================================================
  media_database_is_init
======================================================================*/
BOOL media_database_is_init (const MediaDatabase *self)
  {
  return self->sqlite != NULL;
  }


/*======================================================================
  media_database_set_amd
======================================================================*/
BOOL media_database_set_amd (MediaDatabase *self, 
      const VSMetadata *amd, char **error)
  {
  IN
  char *esc_path = media_database_escape_sql 
    (SAFE (vs_metadata_get_path (amd)));
  char *esc_title = media_database_escape_sql 
    (SAFE (vs_metadata_get_title (amd)));
  char *esc_album = media_database_escape_sql 
    (SAFE (vs_metadata_get_album (amd)));
  char *esc_genre = media_database_escape_sql 
    (SAFE (vs_metadata_get_genre (amd)));
  char *esc_composer = media_database_escape_sql 
    (SAFE (vs_metadata_get_composer (amd)));
  char *esc_artist = media_database_escape_sql 
    (SAFE (vs_metadata_get_artist (amd)));
  char *esc_album_artist = media_database_escape_sql 
    (SAFE (vs_metadata_get_album_artist (amd)));
  char *esc_track = media_database_escape_sql  
    (SAFE (vs_metadata_get_track (amd)));
  char *esc_comment = media_database_escape_sql 
    (SAFE (vs_metadata_get_comment (amd)));
  char *esc_year = media_database_escape_sql 
    (SAFE (vs_metadata_get_year (amd)));
  time_t mtime = vs_metadata_get_mtime (amd);
  size_t size = vs_metadata_get_size (amd);

  char *sql;
  asprintf (&sql, "insert into files "
     "(path,size,mtime,title,album,genre,composer,"
     "artist,album_artist,track,comment,year,exist) values "
     "('%s',%ld,%ld,'%s','%s','%s','%s','%s','%s','%s','%s','%s',1)",
     esc_path,
     size,
     mtime,
     esc_title,
     esc_album,
     esc_genre,
     esc_composer,
     esc_artist,
     esc_album_artist,
     esc_track,
     esc_comment,
     esc_year);

  BOOL ret = media_database_exec (self, sql, error);

  free (sql);
  free (esc_path);
  free (esc_title);
  free (esc_album);
  free (esc_genre);
  free (esc_composer);
  free (esc_artist);
  free (esc_album_artist);
  free (esc_track);
  free (esc_comment);
  free (esc_year);

  OUT
  return ret;
  }

/*======================================================================
  media_database_get_amd
======================================================================*/
VSMetadata *media_database_get_amd (MediaDatabase *self, 
      const char *path)
  {
  IN
  VSMetadata *ret = NULL;

  char *esc_path = media_database_escape_sql (path);

  char *sql;
  asprintf (&sql, "select size,mtime,title,album,genre,composer,artist,"
       "album_artist,track,comment,year from files where path='%s'", esc_path);

  char *error = NULL;

  VSList *list = media_database_sql_query (self, sql, TRUE, 0, &error);
  if (list)
    {
    int l = vs_list_length (list);
    if (l == 11)
      {
      ret = vs_metadata_create();
      vs_metadata_set_path (ret, path);
      vs_metadata_set_size (ret, atoi (vs_list_get (list, 0)));
      vs_metadata_set_mtime (ret, atoi (vs_list_get (list, 1)));
      vs_metadata_set_title (ret, vs_list_get (list, 2));
      vs_metadata_set_album (ret, vs_list_get (list, 3));
      vs_metadata_set_genre (ret, vs_list_get (list, 4));
      vs_metadata_set_composer (ret, vs_list_get (list, 5));
      vs_metadata_set_artist (ret, vs_list_get (list, 6));
      vs_metadata_set_album_artist (ret, vs_list_get (list, 7));
      vs_metadata_set_track (ret, vs_list_get (list, 8));
      vs_metadata_set_comment (ret, vs_list_get (list, 9));
      vs_metadata_set_year (ret, vs_list_get (list, 10));
      }
    else
      {
      vs_log_warning ("Database query for '%s' returned wrong number of columns: got %d, expected 11", path, l);
      } 
    vs_list_destroy (list);
    }
  else
    {
    if (error)
      {
      vs_log_warning ("Database query for '%s' failed: %s", path, error);
      free (error);
      }
    }

  free (sql);
  free (esc_path);
  OUT
  return ret;
  }
 
/*======================================================================
  media_database_get_amd
======================================================================*/
BOOL media_database_has_path (MediaDatabase *self, 
      const char *path)
  {
  IN
  BOOL ret = FALSE;
  char *esc_path = media_database_escape_sql (path);

  char *sql;
  asprintf (&sql, "select path from files where path='%s'", esc_path);

  char *e = NULL;
  char **result = NULL;
  int hits;
  int cols;
  vs_log_debug ("%s: executing SQL %s", __PRETTY_FUNCTION__, sql);
  sqlite3_get_table (self->sqlite, sql, &result, &hits, &cols, &e);
  if (e)
    {
    sqlite3_free (e);
    }
  else
    {
    if (hits > 0) ret = TRUE;
    sqlite3_free_table (result);
    }

  free (sql);
  free (esc_path);
  OUT
  return ret;
  }
 
/*======================================================================
  iterate_all_paths_callback
======================================================================*/
static int iterate_all_paths_callback (void *data, int ncols,
     char **cols, char **dummy)
  {
  (void)dummy;
  BOOL ret = FALSE;
  IteratePathsCallbackData *ipcd = (IteratePathsCallbackData *)data;
  if (ncols == 1)
    {
    const char *path = cols[0];
    ret = ipcd->callback (path, ipcd->user_data);
    }
  else
    {
    // Should never happen
    vs_log_error ("Internal error -- ncols != 1 in %s", __PRETTY_FUNCTION__);
    ret = FALSE;
    }

  if (ret) return 0;
  return -1;
  }

/*======================================================================
  media_database_iterate_all_paths
======================================================================*/
BOOL media_database_iterate_all_paths (MediaDatabase *self,
        DBPathIteratorCallback callback, void *user_data, char **error)
  {
  IN
  BOOL ret = TRUE;

  IteratePathsCallbackData ipcd;
  ipcd.callback = callback;
  ipcd.user_data = user_data;
    
  char *e = NULL;
  sqlite3_exec(self->sqlite, 
    "select path from files", iterate_all_paths_callback,
    &ipcd, &e);

  if (e)
    {
    if (error) (*error) = strdup (e);
    sqlite3_free (e);
    ret = FALSE;
    }
  else
    ret = TRUE;

  OUT
  return ret;
  }

/*======================================================================
  media_database_mark_path_deleted
======================================================================*/
void media_database_mark_path_deleted (MediaDatabase *self, 
        const char *path)
  {
  IN

  char *sql;
  char *esc_path = media_database_escape_sql (path);
  asprintf (&sql, "update files set exist=0 where path='%s'", esc_path);

  media_database_exec_log_error (self, sql);
  free (sql);
  free (esc_path);

  OUT
  }

/*======================================================================
  media_database_remove_nonexistent
======================================================================*/
void media_database_remove_nonexistent (MediaDatabase *self)
  {
  IN
  media_database_exec_log_error (self, "delete from files where exist=0");
  OUT
  }

/*======================================================================
  media_database_delete_by_path
======================================================================*/
void media_database_delete_by_path (MediaDatabase *self, const char *path)
  {
  IN
  char *sql;
  char *esc_path = media_database_escape_sql 
    (SAFE (path));
  asprintf (&sql, "delete from files where path='%s'", esc_path);
  media_database_exec_log_error 
    (self, sql);
  free (esc_path);
  free (sql);
  OUT
  }

/*======================================================================
  media_database_get_filename
======================================================================*/
const char *media_database_get_filename (const MediaDatabase *self)
  {
  return self->mdb_file;
  }

