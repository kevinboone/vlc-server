/*======================================================================
  
  vlc-rest-server

  server/src/gui_files.c

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <microhttpd.h>
#include <vlc-server/vs_log.h> 
#include <vlc-server/vs_util.h> 
#include "template_manager.h"
#include "http_util.h"
#include "gui_files.h"

/*======================================================================
  
  gui_files_encode_for_js

======================================================================*/
VSString *gui_files_encode_for_js (const char *file)
  {
  IN
  VSString *s = vs_string_create(file);
  vs_string_substitute_all_in_place (s, "\'", "\\'");
  VSString *ret = vs_string_encode_url (vs_string_cstr (s));
  vs_string_destroy (s);
  OUT
  return ret;
  }

/*======================================================================
  
  gui_files_file_cell

======================================================================*/
void gui_files_file_cell (VSString *body, const char *file)
  {
  IN
      const char *file_file = file;
      const char *p = strrchr (file, '/');
      if (p) file_file = p + 1;
      VSString *enc_file = gui_files_encode_for_js (file); 
      vs_string_append (body, "<div>");
      vs_string_append (body, "<a href=\"javascript:cmd_add('");
      vs_string_append (body, vs_string_cstr (enc_file)); 
      vs_string_append (body, "')\">[add]</a> ");
      vs_string_append (body, "<a href=\"javascript:cmd_play('");
      vs_string_append (body, vs_string_cstr (enc_file)); 
      vs_string_append (body, "')\">[play]</a> ");
      vs_string_append (body, file_file);
      vs_string_append (body, "</div>\n");
      vs_string_destroy (enc_file); 
  OUT
  }

/*======================================================================
  
  gui_files_dir_cell

======================================================================*/
void gui_files_dir_cell (VSString *body, const char *dir, BOOL covers)
  {
  IN
      const char *dir_file = dir;
      const char *p = strrchr (dir, '/');
      if (p) dir_file = p + 1;
      VSString *enc_dir = gui_files_encode_for_js (dir); 
      VSString *enc_dir_html = vs_string_encode_url (dir); 
      if (covers)
        vs_string_append (body, "<div class=\"dircell_cover\">");
      else
        vs_string_append (body, "<div class=\"dircell_nocover\">");

      if (covers)
        {
        vs_string_append (body, "<a href=\"");

        vs_string_append (body, "/gui/files");
        // TODO -- do we need to URL-encode 'dir' ??
        vs_string_append (body, vs_string_cstr (enc_dir_html));
        vs_string_append (body, "?covers=1\">");

        vs_string_append (body, "<img class=\"coverimg\" src=\"/cover/");
        vs_string_append (body, vs_string_cstr (enc_dir_html)); 
        vs_string_append (body, "\">");
        vs_string_append (body, "</a>");
        }

      if (covers)
        vs_string_append (body, "<div>");

      vs_string_append (body, "<a href=\"javascript:cmd_add('");
      vs_string_append (body, vs_string_cstr (enc_dir)); 
      vs_string_append (body, "')\">[add all]</a> ");
      vs_string_append (body, "<a href=\"javascript:cmd_play('");
      vs_string_append (body, vs_string_cstr (enc_dir)); 
      vs_string_append (body, "')\">[play all]</a> ");

      if (covers)
        vs_string_append (body, "</div>");

      //vs_string_append (body, "<div>");
      //vs_string_append (body, "</div>");

      vs_string_append (body, "<a href=\"");
      vs_string_append (body, "/gui/files");
      vs_string_append (body, dir);
      if (covers)
        vs_string_append (body, "?covers=1");
      vs_string_append (body, "\">");
      vs_string_append (body, dir_file);
      vs_string_append (body, "</a>");
      vs_string_append (body, "</div>\n");
      vs_string_destroy (enc_dir);
      vs_string_destroy (enc_dir_html);
  OUT
  }


/*======================================================================
  
  gui_files_get_body

======================================================================*/
VSString *gui_files_get_body (const Player *player, 
            const char *media_root, const char *path, 
            const VSProps *arguments, int count)
  {
  IN
  vs_log_debug ("GUI file list for %s", path);
  if (!path[0]) path = "/";

  int start = 0;
  int covers = 0; // Boolean -- only 0 or 1, but used as an int in headers
  const char *s_start = vs_props_get (arguments, "start");
  const char *s_count = vs_props_get (arguments, "count");
  const char *s_covers = vs_props_get (arguments, "covers");
  if (s_count) count = atoi (s_count);
  if (s_covers) covers = atoi (s_covers);
  if (count == 0) count = 30; // TODO
  if (s_start) start = atoi (s_start);

  VSString *body = vs_string_create ("");
  vs_string_append (body, "<h1>Showing files in ");
  vs_string_append (body, path);
  vs_string_append (body, "</h1>\n");
  vs_string_append (body, "<p>");
  if (covers)
    vs_string_append (body, "<a href=\"/gui/files?covers=1\">");
  else
    vs_string_append (body, "<a href=\"/gui/files/\">");
  vs_string_append (body, "[top]</a>");
  vs_string_append (body, "</p>\n");
  VSApiError e = 0;
  VSList *dirs = api_list_dirs (player, media_root, path, &e);
  if (dirs)
    {
    int l = vs_list_length (dirs);

    int dirs_per_page = count;

    int pages = l / dirs_per_page;
    int this_page = start / dirs_per_page;
    int prev_page_start = start - count;
    if (prev_page_start < 0) prev_page_start = 0; 
    int next_page_start = start + count;
    if (next_page_start >= l) next_page_start = l - count;

    char ss[200];
    
    // 'prev' link
    if (start != 0)
      {
      sprintf (ss, 
          "<a href=\"/gui/files?start=%d&count=%d&covers=%d\">prev </a>", 
          prev_page_start, dirs_per_page, covers);
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
        sprintf (ss, 
          "<a href=\"/gui/files?start=%d&count=%d&covers=%d\">%d </a>", 
          i * dirs_per_page, dirs_per_page, covers, i);
        }
      else
        {
        sprintf (ss, "%d ", i);
        }
      vs_string_append (body, ss);
      }

    // 'next' link
    if (start < l - count)
      {
      sprintf (ss, 
          "<a href=\"/gui/files?start=%d&count=%d&covers=%d\">next </a>", 
          next_page_start, dirs_per_page, covers);
      }
    else
      {
      sprintf (ss, "next ");
      }
    vs_string_append (body, ss);
    vs_string_append (body, "<p/>\n");

    for (int i = start; i < start + count && i < l; i++)
      {
      const char *dir = vs_list_get (dirs, i);
      gui_files_dir_cell (body, dir, covers);
      }
    vs_list_destroy (dirs);
    }
  else
    {
    vs_string_append (body, "<p>");
    vs_string_append (body, vs_util_strerror (e));
    vs_string_append (body, "</p>");
    }

  VSList *files = api_list_files (player, media_root, path, &e);
  if (files)
    {
    int l = vs_list_length (files);
    for (int i = 0; i < l; i++)
      {
      const char *file = vs_list_get (files, i);
      gui_files_file_cell (body, file);
      }
    vs_list_destroy (files);
    }
  else
    {
    vs_string_append (body, "<p>");
    vs_string_append (body, vs_util_strerror (e));
    vs_string_append (body, "</p>");
    }



  OUT
  return body;
  }

