/*======================================================================
  
  vlc-server

  scanner.c

  Copyright (c)2023 Kevin Boone, GPL v3.0

======================================================================*/
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <vlc-server/vs_log.h> 
#include "http_server.h" 
#include "player.h" 
#include "media_database.h" 
#include "scanner.h" 
#include "file.h" 
#include "tag_reader.h" 

typedef struct _ScannerIteratorContext 
  {
  const char *media_root;
  MediaDatabase *mdb;
  int nonexistent;
  } ScannerIteratorContext;

// We might as well make this static, since it won't change
//   at run-time
static char scanner_progress_file [PATH_MAX + 1];

/*======================================================================
  
  append_sep 

======================================================================*/
static void append_sep (char *path)
  {
  int l = strlen (path);
  if (l == 0)
    {
    strcat (path, "/");
    return;
    }
  if (path[l - 1] != '/')
    strcat (path, "/");
  }

/*======================================================================
  
  has_audio_extension 

======================================================================*/
static BOOL has_audio_extension (const char *path, 
        char *const *extensions, int num_extensions)
  {
  const char *p = strrchr (path, '.');
  if (!p) return FALSE;

  p++;
  for (int i = 0; i < num_extensions; i++)
    {
    if (strcasecmp (p, extensions[i]) == 0) return TRUE;
    }

  return FALSE;
  }

/*======================================================================
  
  scanner_write_cover

======================================================================*/
static void scanner_write_cover (const unsigned char *data, int len, 
             const char *mime, const char *dir)
  {
  IN
  char *path;

  const char *cover_filename = NULL;
  if (strcmp (mime, "image/png") == 0)
    cover_filename = "cover.png";
  else if (strcmp (mime, "image/gif") == 0)
    cover_filename = "cover.gif";
  else if (strcmp (mime, "image/jpeg") == 0)
    cover_filename = "cover.jpg";
  if (cover_filename)
    {
    asprintf (&path, "%s/%s", dir, cover_filename);
    if (access (path, R_OK) != 0)
      {
      int f = open (path, O_CREAT | O_WRONLY | O_TRUNC, 0640);
      if (f)
        {
        write (f, data, len);
        close (f);
        }
      else
        {
        vs_log_error ("Can't write cover: %s: %s", 
           path, strerror (errno));
        }
      }
    }
  free (path);
  OUT
  }

/*======================================================================
  
  scanner_update_progress

======================================================================*/
void scanner_update_progress (int num)
  {
  if (scanner_progress_file[0])
    {
    int f = open (scanner_progress_file, O_WRONLY | O_TRUNC, 0644);
    if (f >= 0)
      {
      char s[20];
      sprintf (s, "%d", num);
      write (f, s, strlen (s));
      close (f);
      }
    }
  }
 
/*======================================================================
  
  scanner_create_progress_file

======================================================================*/
void scanner_create_progress_file (void)
  {
  scanner_get_progress_file (scanner_progress_file, 
    sizeof (scanner_progress_file));
  int f = open (scanner_progress_file, O_CREAT | O_WRONLY | O_TRUNC, 0644);
  if (f >= 0)
    close (f);
  else
    vs_log_error ("Can't create progress file '%s': '%s'", 
      scanner_progress_file, strerror (errno));
  }
 
/*======================================================================
  
  scanner_delete_progress_file

======================================================================*/
void scanner_delete_progress_file (void)
  {
  unlink (scanner_progress_file);
  }
 
/*======================================================================
  
  scanner_should_update

======================================================================*/
static BOOL scanner_should_update (MediaDatabase *mdb, 
         const char *rel_path, const char *abs_path, BOOL *exists)
  {
  *exists = FALSE;
  // If the file is not in the database, we should always update
  if (!media_database_has_path (mdb, rel_path)) return TRUE;
  VSMetadata *amd = media_database_get_amd (mdb, rel_path);
  // If get metadata fails, we also update; but this should never
  //   happen
  if (!amd) return TRUE;
  time_t stored_mtime = vs_metadata_get_mtime (amd);
  vs_metadata_destroy (amd);

  struct stat sb;

  // There's no point updating the database if the file can't
  //   actually be read
  if (stat (abs_path, &sb)) return FALSE;
  vs_log_debug ("stored mtime=%ld file_mtime=%ld \n", stored_mtime, sb.st_mtime);
  // Update if the file is more than three seconds more recent than the
  //   stored time. VFAT filesystems have ~2 sec timestamp precision, and
  //   we don't want to update for nothing. 
  if (sb.st_mtime - stored_mtime > 3) 
    {
    vs_log_debug ("%s is nore recent", abs_path);
    *exists = TRUE;
    return TRUE;
    }
  return FALSE;
  }


/*======================================================================
  
  scanner_do_file

======================================================================*/
static void scanner_do_file (const char *abs_path, 
         const char *rel_path, const char *dir, BOOL full, 
         MediaDatabase *mdb, char * const *extensions, int num_extensions, 
         int *considered, int *added)
  {
  IN
  // We don't store the leading / in the database -- it's just a waste
  //   of space
  if (rel_path[0] == '/') rel_path++;
  if (has_audio_extension (abs_path, extensions, num_extensions))
    {
    vs_log_debug ("Scanner considering file %s", abs_path);
    (*considered)++;
    if ((*considered) % 10 == 0)
      scanner_update_progress (*considered);
    BOOL update_db = FALSE;
    if (full || scanner_should_update (mdb, rel_path, abs_path, &update_db))
      {
      TagData *tags; 
      if (tag_get_tags (abs_path, &tags) == TAG_OK)
        {
        VSMetadata *amd = vs_metadata_create ();
        vs_metadata_set_path (amd, rel_path);
        const char *s = (char *)tag_get_common (tags, TAG_COMMON_ALBUM);
        vs_metadata_set_title (amd, s);
        s = (char *)tag_get_common (tags, TAG_COMMON_TITLE);
        vs_metadata_set_title (amd, s);
        s = (char *)tag_get_common (tags, TAG_COMMON_ALBUM);
        vs_metadata_set_album (amd, s);
        s = (char *)tag_get_common (tags, TAG_COMMON_ARTIST);
        vs_metadata_set_artist (amd, s);
        s = (char *)tag_get_common (tags, TAG_COMMON_ALBUM_ARTIST);
        vs_metadata_set_album_artist (amd, s);
        s = (char *)tag_get_common (tags, TAG_COMMON_COMPOSER);
        vs_metadata_set_composer (amd, s);
        s = (char *)tag_get_common (tags, TAG_COMMON_YEAR);
        vs_metadata_set_year (amd, s);
        s = (char *)tag_get_common (tags, TAG_COMMON_GENRE);
        vs_metadata_set_genre (amd, s);
        s = (char *)tag_get_common (tags, TAG_COMMON_TRACK);
        vs_metadata_set_track (amd, s);
        s = (char *)tag_get_common (tags, TAG_COMMON_COMMENT);
        vs_metadata_set_comment (amd, s);

        struct stat sb;
        if (stat (abs_path, &sb) == 0)
          {
          vs_metadata_set_mtime (amd, sb.st_mtime);
          vs_metadata_set_size (amd, sb.st_size);
          }

        char *error = NULL;
        if (full || update_db)
          media_database_delete_by_path (mdb, rel_path);
        media_database_set_amd (mdb, amd, &error);
        if (error)
          {
          vs_log_error ("Can't insert into database: %s: %s",
             abs_path, error);
          free (error);
          }
        else
          (*added)++;
        vs_metadata_destroy (amd);

        if (tags->cover) 
          {
          scanner_write_cover (tags->cover, tags->cover_len, 
             tags->cover_mime, dir);
          }

        tag_free_tag_data (tags);
        }
      else
        {
        vs_log_debug ("Can't read metadata from %s", abs_path);
        }
      }
    else
      {
      vs_log_debug ("File %s already in database", abs_path);
      }
    }
  else
    vs_log_debug ("File %s does not have an audio extension", abs_path);

  OUT
  }

/*======================================================================
  
  scanner_run_dir

======================================================================*/
static void scanner_run_dir (const char *root, const char *dirname, 
         BOOL full, int depth, MediaDatabase *mdb, char* const *extensions,
         int num_extensions, int *considered, int *added)
  {
  IN

  char fs_dir[PATH_MAX];
  strcpy (fs_dir, root);
  append_sep (fs_dir);
  strcat (fs_dir, dirname);
  vs_log_info ("Scanning directory %s", fs_dir);

  DIR *dir = opendir (fs_dir);
  if (dir)
    {
    struct dirent *de = readdir (dir);
    while (de)
      {
      if (de->d_name[0] != '.') 
        {
        char fs_path[PATH_MAX];
        strcpy (fs_path, fs_dir);
        append_sep (fs_path);
        strcat (fs_path, de->d_name);
        char rel_path[PATH_MAX];
        strcpy (rel_path, dirname);
        append_sep (rel_path);
        strcat (rel_path, de->d_name);
        struct stat sb;
        if (stat (fs_path, &sb) == 0)
          {
          if (S_ISREG (sb.st_mode) || S_ISLNK (sb.st_mode))
            {
            scanner_do_file (fs_path, rel_path, fs_dir, full, mdb, extensions, 
               num_extensions, considered, added);
            }
          else if (S_ISDIR (sb.st_mode))
            {
            scanner_run_dir (root, rel_path, full, depth + 1, mdb, 
              extensions, num_extensions, considered, added);
            }
          }
        else
          vs_log_warning ("Stat failed: %s: %s", fs_path, strerror (errno));
        }
        
      de = readdir (dir);
      }
    closedir (dir);
    }
  else
    {
    vs_log_warning ("Can't open directory %s: %s", dir, strerror (errno));
    }
  OUT
  }
 
/*======================================================================
  
  scanner_check_file_exists

======================================================================*/
BOOL scanner_check_file_exists (const char *path, void *data)
  {
  ScannerIteratorContext *sic = data;
  const char *media_root = sic->media_root;
  MediaDatabase *mdb = sic->mdb;
  char fullpath[PATH_MAX + 1];
  snprintf (fullpath, PATH_MAX, "%s/%s", media_root, path);
  if (access (fullpath, R_OK) == 0)
    {
    }
  else
    {
    // File is in the database, but not on disk. But don't delete
    //   paths that begin with '-', as these are literal streams,
    //   not derived from the scanner.
    if (path[0] != '=')
      {
      media_database_mark_path_deleted (mdb, path);
      (sic->nonexistent)++;
      }
    }
  return TRUE;
  }

/*======================================================================
  
  scanner_run 

======================================================================*/
void scanner_run (const char *media_root, BOOL full, MediaDatabase *mdb,
       const char *patterns)
  {
  IN

  const int MAX_PATTERNS = 20;

  char *lockfile;
  const char *mdb_file = media_database_get_filename (mdb);
  asprintf (&lockfile, "%s.lock", mdb_file);
  if (access (lockfile, R_OK) == 0)
    {
    // Scanner is already running
    vs_log_error ("Scanner seems to be running already");
    vs_log_error ("Delete '%s' if you are sure it isn't", lockfile);
    }
  else
    {
    int f = open (lockfile, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    close (f);
    
    char *patterns2 = strdup (patterns);
    char **extensions = malloc (sizeof (char *) * (MAX_PATTERNS + 1));

    int num_patterns = 0;
    char *tok = strtok (patterns2, ",");
    if (tok)
      {
      do
	{
	char *p = strrchr (tok, '.');
	if (p)
	  {
	  extensions[num_patterns] = strdup (p + 1);
	  num_patterns++;
	  }
	} while ((tok = strtok (NULL, ",")) && num_patterns < MAX_PATTERNS);
      }
    extensions[num_patterns] = NULL;

    if (full)
      vs_log_info ("Starting full scan");
    else
      vs_log_info ("Starting quick scan");

    int considered = 0;
    int added = 0;

    scanner_create_progress_file();

    scanner_run_dir (media_root, "", full, 0, mdb, extensions, num_patterns,
       &considered, &added);
    
    vs_log_info ("Checking for deleted files");

    char *error = NULL;
    ScannerIteratorContext sic;
    sic.media_root = media_root;
    sic.mdb = mdb;
    sic.nonexistent = 0;
    if (!media_database_iterate_all_paths (mdb, scanner_check_file_exists, 
             &sic, &error))
      {
      vs_log_error (error);
      free (error);
      }

    media_database_remove_nonexistent (mdb);

    scanner_delete_progress_file();

    vs_log_info ("Scanner finished");
    vs_log_info ("Considered %d files", considered);
    vs_log_info ("Added %d file(s) to database", added);
    vs_log_info ("Deleted %d missing file(s) from database", sic.nonexistent);

    for (int i = 0; i < num_patterns; i++)
      free (extensions[i]);
    free (patterns2);
    free (extensions);
    unlink (lockfile);
    }
  free (lockfile);
  OUT
  }

/*======================================================================
  
  scanner_get_progress_file

======================================================================*/
void scanner_get_progress_file (char *file, int len)
  {
  const char *tmp = getenv("TMP");
  if (!tmp) tmp = "/tmp";
  snprintf (file, len - 1, "%s/" VLC_SCAN_PROG_FILE, tmp);
  }

/*======================================================================
  
  scanner_get_progress

======================================================================*/
int scanner_get_progress (void)
  {
  int ret = -1;

  char filename[PATH_MAX + 1];
  scanner_get_progress_file (filename, PATH_MAX);

  int f = open (filename, O_RDONLY);
  if (f >= 0)
    {
    char s[20];
    int n = read (f, s, sizeof (s) - 1);
    if (n > 0)
      {
      s[n] = 0;
      ret = atoi (s);
      }
    close (f);
    }

  return ret;
  }



