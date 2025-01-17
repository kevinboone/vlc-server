/*======================================================================
  
  vlc-server

  server/src/gui.c

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <microhttpd.h>
#include <vlc-server/vs_log.h> 
#include "template_manager.h"
#include "http_util.h"
#include "gui.h"
#include "gui_files.h"
#include "gui_playlist.h"
#include "gui_albums.h"
#include "gui_tracks.h"
#include "gui_artists.h"
#include "gui_album_artists.h"
#include "gui_composers.h"
#include "gui_genres.h"
#include "gui_streams.h"
#include "gui_home.h"
#include "gui_help.h"
#include "gui_strictsearch.h"
#include "gui_search.h"
#include "gui_tracks.h"
#include "media_database.h"

/*======================================================================
  idiv_round_up 
======================================================================*/
static int idiv_round_up (int numerator, int denominator)
  {
  return numerator / denominator
      + (((numerator < 0) ^ (denominator > 0)) && (numerator%denominator));
  }

/*======================================================================
  gui_process_request
======================================================================*/
BOOL gui_process_request (Player *player, const char *media_root, 
      const char *url, BYTE **buff, int *len, const VSProps *arguments,
      int matches_per_page, MediaDatabase *mdb, const char *instance_name)
  {
  IN
  BOOL ret = FALSE;

  VSString *s = template_manager_get_string_by_tag ("generic.html"); 
  if (s)
    {
    if (strncmp (url, "home", 4) == 0)
      {
      VSString *body = gui_home_get_body (url + 4, arguments, instance_name);
      template_manager_substitute_placeholder 
          (s, "BODY", vs_string_cstr (body));
      template_manager_substitute_placeholder 
          (s, "TITLE", NAME " - Home"); 
      vs_string_destroy (body);
      }
    else if (strncmp (url, "help", 4) == 0)
      {
      VSString *body = gui_help_get_body (url + 4, arguments, instance_name);
      template_manager_substitute_placeholder 
          (s, "BODY", vs_string_cstr (body));
      template_manager_substitute_placeholder 
          (s, "TITLE", NAME " - Home"); 
      vs_string_destroy (body);
      }
    else if (strncmp (url, "files/", 5) == 0)
      {
      VSString *body = gui_files_get_body (player, media_root, url + 5,
        arguments, matches_per_page);
      template_manager_substitute_placeholder 
          (s, "BODY", vs_string_cstr (body));
      template_manager_substitute_placeholder 
          (s, "TITLE", NAME " - Files");
      vs_string_destroy (body);
      }
    else if (strncmp (url, "playlist", 8) == 0)
      {
      VSString *body = gui_playlist_get_body (player, media_root);
      template_manager_substitute_placeholder 
          (s, "BODY", vs_string_cstr (body));
      template_manager_substitute_placeholder 
          (s, "TITLE", NAME " - Playlist"); 
      vs_string_destroy (body);
      }
    else if (strncmp (url, "album_artists", 13) == 0)
      {
      VSString *body = gui_album_artists_get_body (url + 13,
        arguments, matches_per_page, mdb);
      template_manager_substitute_placeholder 
          (s, "BODY", vs_string_cstr (body));
      template_manager_substitute_placeholder 
          (s, "TITLE", NAME " - Albums");
      vs_string_destroy (body);
      }
    else if (strncmp (url, "albums", 6) == 0)
      {
      VSString *body = gui_albums_get_body (url + 6,
        arguments, matches_per_page, mdb);
      template_manager_substitute_placeholder 
          (s, "BODY", vs_string_cstr (body));
      template_manager_substitute_placeholder 
          (s, "TITLE", NAME " - Albums");
      vs_string_destroy (body);
      }
    else if (strncmp (url, "tracks", 6) == 0)
      {
      VSString *body = gui_tracks_get_body (url + 6,
        arguments, matches_per_page, mdb);
      template_manager_substitute_placeholder 
          (s, "BODY", vs_string_cstr (body));
      template_manager_substitute_placeholder 
          (s, "TITLE", NAME " - Tracks"); 
      vs_string_destroy (body);
      }
    else if (strncmp (url, "artists", 7) == 0)
      {
      VSString *body = gui_artists_get_body (url + 7,
        arguments, matches_per_page, mdb);
      template_manager_substitute_placeholder 
          (s, "BODY", vs_string_cstr (body));
      template_manager_substitute_placeholder 
          (s, "TITLE", NAME " - Artists"); 
      vs_string_destroy (body);
      }
    else if (strncmp (url, "genres", 6) == 0)
      {
      VSString *body = gui_genres_get_body (url + 6,
        arguments, matches_per_page, mdb);
      template_manager_substitute_placeholder 
          (s, "BODY", vs_string_cstr (body));
      template_manager_substitute_placeholder 
          (s, "TITLE", NAME " - Genres"); 
      vs_string_destroy (body);
      }
    else if (strncmp (url, "composers", 9) == 0)
      {
      VSString *body = gui_composers_get_body (url + 9,
        arguments, matches_per_page, mdb);
      template_manager_substitute_placeholder 
          (s, "BODY", vs_string_cstr (body));
      template_manager_substitute_placeholder 
          (s, "TITLE", NAME " - Composers"); 
      vs_string_destroy (body);
      }
    else if (strncmp (url, "streams", 7) == 0)
      {
      VSString *body = gui_streams_get_body (url + 7,
        arguments, matches_per_page, mdb);
      template_manager_substitute_placeholder 
          (s, "BODY", vs_string_cstr (body));
      template_manager_substitute_placeholder 
          (s, "TITLE", NAME " - Streams"); 
      vs_string_destroy (body);
      }
    else if (strncmp (url, "strictsearch", 12) == 0)
      {
      VSString *body = gui_strictsearch_get_body (mdb, 
           arguments, 10 /* todo */);;
      template_manager_substitute_placeholder 
          (s, "BODY", vs_string_cstr (body));
      template_manager_substitute_placeholder 
          (s, "TITLE", NAME " - Composers"); 
      vs_string_destroy (body);
      }
    else if (strncmp (url, "search", 6) == 0)
      {
      VSString *body = gui_search_get_body (mdb, 
           arguments, 10 /* todo */);;
      template_manager_substitute_placeholder 
          (s, "BODY", vs_string_cstr (body));
      template_manager_substitute_placeholder 
          (s, "TITLE", NAME " - Composers"); 
      vs_string_destroy (body);
      }
    else
      { 
      template_manager_substitute_placeholder (s, "BODY", "Not found");
      }

    *buff = (BYTE *) vs_string_cstr (s);
    vs_string_destroy_wrapper (s);
    *len = strlen ((char *)*buff);
    ret = TRUE;
    }

  OUT
  return ret;
  }

/*======================================================================
  gui_set_constraints_from_arguments
======================================================================*/
void gui_set_constraints_from_arguments 
       (VSSearchConstraints *constraints, const VSProps *arguments)
  {
  constraints->start = 0;
  const char *s_start = vs_props_get (arguments, "start");
  const char *s_count = vs_props_get (arguments, "count");
  const char *s_where = vs_props_get (arguments, "where");
  char *sql_where = NULL;
  // Convert JS \' to SQL ''
  if (s_where)
    {
    sql_where = strdup (s_where);
    char *p = strstr (sql_where, "\\'");
    if (p)
      {
      *p = '\'';
      } 
    } 
  if (s_count) constraints->count = atoi (s_count);
  if (constraints->count == 0) constraints->count = 30; // TODO
  if (s_start) constraints->start = atoi (s_start);
  if (sql_where) vs_search_constraints_set_where (constraints, sql_where);
  if (sql_where) free (sql_where);
  //if (s_where) printf ("where=%s\n", s_where);
  }

/*======================================================================
  gui_write_page_navigation
======================================================================*/
static void gui_write_page_navigation (VSString *body, 
       const char *field_name, const VSProps *arguments, int count,
       int total_count)
  {
  char ss[200];

  int covers = 0; // FALSE
  const char *s_covers = vs_props_get (arguments, "covers");
  if (s_covers) covers = atoi (s_covers);
  const char *where = vs_props_get (arguments, "where"); // Can be null

  int start = 0;
  const char *s_start = vs_props_get (arguments, "start");
  const char *s_count = vs_props_get (arguments, "count");
  if (s_count) count = atoi (s_count);
  if (s_covers) covers = atoi (s_covers);
  if (count == 0) count = 30; // TODO
  if (s_start) start = atoi (s_start);

  int entries_per_page = count;
  int pages = idiv_round_up (total_count,  entries_per_page);
  int this_page = start / entries_per_page;
  int prev_page_start = start - count;
  if (prev_page_start < 0) prev_page_start = 0; 
  int next_page_start = start + count;
  if (next_page_start >= total_count) next_page_start = total_count - count;

  if (total_count > count)
    {
    // 'prev' link
    if (start != 0)
      {
      if (where)
	snprintf (ss, sizeof (ss) - 1, 
	   "<a href=\"/gui/%s?start=%d&count=%d&covers=%d&where=%s\">prev </a>", 
	   field_name, prev_page_start, entries_per_page, covers, where);
      else
	snprintf (ss, sizeof (ss) - 1, 
	   "<a href=\"/gui/%s?start=%d&count=%d&covers=%d\">prev </a>", 
	   field_name, prev_page_start, entries_per_page, covers);

      }
    else
      {
      sprintf (ss, "prev ");
      }
    vs_string_append (body, ss);

    // Specific page links
    for (int i = 0; i < pages; i++)
      {
      if (i != this_page)
	{ 
	if (where)
	  snprintf (ss, sizeof (ss) - 1,
	    "<a href=\"/gui/%s?start=%d&count=%d&covers=%d&where=%s\">%d </a>", 
	    field_name, i * entries_per_page, entries_per_page, covers, where, i);
	else
	  snprintf (ss, sizeof (ss) - 1, 
	    "<a href=\"/gui/%s?start=%d&count=%d&covers=%d\">%d </a>", 
	    field_name, i * entries_per_page, entries_per_page, covers, i);
	}
      else
	{
	sprintf (ss, "%d ", i);
	}

      vs_string_append (body, ss);
      }
    
    // 'next' link
    if (start < total_count - count)
      {
      if (where)
	snprintf (ss, sizeof (ss) - 1, 
	   "<a href=\"/gui/%s?start=%d&count=%d&covers=%d&where=%s\">next </a>", 
	   field_name, next_page_start, entries_per_page, covers, where);
      else
	snprintf (ss, sizeof (ss) - 1, 
	   "<a href=\"/gui/%s?start=%d&count=%d&covers=%d\">next </a>", 
	   field_name, next_page_start, entries_per_page, covers);

      }
    else
      {
      sprintf (ss, " next");
      }
    vs_string_append (body, ss);
    }
  }


/*======================================================================
  gui_get_results_page
======================================================================*/
VSString *gui_get_results_page (const char *path, 
            const VSProps *arguments, int count, MediaDatabase *mdb, 
            const char *field_name, const char *header, 
            MediaDatabaseColumn col, 
            BOOL headers, GUICellCallback cell_callback, BOOL tabular)
  {
  (void)path;
  IN
  VSString *body = vs_string_create ("");

  if (headers)
    {
    vs_string_append (body, "<h1>");
    vs_string_append (body, header);
    vs_string_append (body, "</h1>\n");
    }

  if (media_database_is_init (mdb))
    {
    int covers = 0; // FALSE
    const char *s_covers = vs_props_get (arguments, "covers");
    if (s_covers) covers = atoi (s_covers);
    VSSearchConstraints *constraints = vs_search_constraints_new();
    gui_set_constraints_from_arguments (constraints, arguments);
    VSList *entries = vs_list_create (free);
    char *error = NULL;

    media_database_search (mdb, col, entries, constraints, &error);

    if (error == NULL)
      {
      int total_count = media_database_search_count (mdb, col, 
        constraints, &error);

      if (total_count > 0)
        { 
	int start = 0;
	const char *s_start = vs_props_get (arguments, "start");
	const char *s_count = vs_props_get (arguments, "count");
	const char *s_covers = vs_props_get (arguments, "covers");
	const char *where = vs_props_get (arguments, "where");
	if (s_count) count = atoi (s_count);
	if (s_covers) covers = atoi (s_covers);
	if (count == 0) count = 30; // TODO
	if (s_start) start = atoi (s_start);

	int l = vs_list_length (entries);

	if (headers)
	  {
	  vs_string_append_printf (body, "<h3>%d to %d of %d", start + 1, 
	     start + count < total_count 
	       ? start + count : total_count, total_count);
	  if (where)
	    {
	    vs_string_append (body, " (");
	    vs_string_append (body, where);
	    vs_string_append (body, ")");
	    }
	  vs_string_append (body, "</h3>");
	  }

	int prev_page_start = start - count;
	if (prev_page_start < 0) prev_page_start = 0; 
	int next_page_start = start + count;
	if (next_page_start >= total_count) next_page_start = total_count - count;

	gui_write_page_navigation (body, field_name, arguments, 
	   count, total_count);

	vs_string_append (body, "<p/>\n");

	if (tabular)
	  vs_string_append (body, "<table class=\"resulttable\">\n");
	for (int i = 0; i < l; i++)
	  {
	  const char *entry = vs_list_get (entries, i);
	  cell_callback (body, entry, covers, mdb);
	  }
	if (tabular)
	  vs_string_append (body, "</table>\n");
        }
      else
        {
	vs_string_append (body, "No matches found\n");
        }
      }
    else
      {
      vs_string_append (body, error);
      vs_string_append (body, "\n");
      free (error);
      }
    vs_list_destroy (entries);
    vs_search_constraints_destroy (constraints);
    }
  else
    {
    vs_log_error (MDB_ERR_NOT_INIT);
    vs_string_append (body, MDB_ERR_NOT_INIT);
    }
  OUT
  return body;
  }


/*======================================================================
  gui_get_limited_results_page
======================================================================*/
VSString *gui_get_limited_results_page (
            const VSProps *arguments, int count, MediaDatabase *mdb, 
            const char *field_name, const char *header, 
            MediaDatabaseColumn col, 
            BOOL headers, GUICellCallback cell_callback, BOOL tabular)
  {
  IN
  VSString *body = vs_string_create ("");

  if (headers)
    {
    vs_string_append (body, "<h1>");
    vs_string_append (body, header);
    vs_string_append (body, "</h1>\n");
    }

  if (media_database_is_init (mdb))
    {
    int covers = 0; // FALSE
    const char *s_covers = vs_props_get (arguments, "covers");
    if (s_covers) covers = atoi (s_covers);
    VSSearchConstraints *constraints = vs_search_constraints_new();
    gui_set_constraints_from_arguments (constraints, arguments);
    constraints->start = 0;
    constraints->count = count;
    VSList *entries = vs_list_create (free);
    char *error = NULL;

    media_database_search (mdb, col, entries, constraints, &error);

    if (error == NULL)
      {
      int total_count = media_database_search_count (mdb, col, 
        constraints, &error);

      if (total_count > 0)
        { 

	int l = vs_list_length (entries);

	if (headers)
	  {
	  vs_string_append_printf (body, "<p>Showing %d of %d ", 
               l, total_count);
	  }

        if (l != total_count)
          {
	  const char *where = vs_props_get (arguments, "where");
	  char *all_link;
	  asprintf (&all_link, "/gui/%s?where=%s&covers=%d", 
	    field_name, where, covers);
	  vs_string_append (body, "<a href=\"");
	  vs_string_append (body, all_link);
	  vs_string_append (body, "\">[show all]</a><br/>\n");
	  free (all_link);
          }
	vs_string_append (body, "</p>\n");

	vs_string_append (body, "<p/>\n");

	if (tabular)
	  vs_string_append (body, "<table class=\"resulttable\">\n");
	for (int i = 0; i < l; i++)
	  {
	  const char *entry = vs_list_get (entries, i);
	  cell_callback (body, entry, covers, mdb);
	  }
	if (tabular)
	  vs_string_append (body, "</table>\n");
        }
      else
        {
	vs_string_append (body, "<p>No matches found</p>\n");
        }
      }
    else
      {
      vs_string_append (body, error);
      vs_string_append (body, "\n");
      free (error);
      }
    vs_list_destroy (entries);
    vs_search_constraints_destroy (constraints);
    }
  else
    {
    vs_log_error (MDB_ERR_NOT_INIT);
    vs_string_append (body, MDB_ERR_NOT_INIT);
    }
  OUT
  return body;
  }




