/*======================================================================
  
  vlc-server

  server/src/gui_album_artists2.c

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
#include "gui.h"
#include "gui_album_artists2.h"
#include "media_database.h"

#define SAFE(x)((x) ? (x) : "")

/*======================================================================
  gui_album_artists2_get_body
======================================================================*/
VSString *gui_album_artists2_get_body (const char *path, 
            const VSProps *arguments, int count, MediaDatabase *mdb)
  {
  (void)path;
  (void)count;
  (void)mdb;
  const char *s_covers = vs_props_get (arguments, "covers"); 
  const char *s_start = vs_props_get (arguments, "start"); 
  const char *s_where = vs_props_get (arguments, "where"); 

  VSString *body = vs_string_create_empty();
  vs_string_append (body, "<h2>Album artists</h2>\n");
  if (s_where)
    {
    vs_string_append (body, "<h3>");
    vs_string_append (body, s_where); 
    vs_string_append (body, "</h3>\n");
    }

  vs_string_append (body, "<div id=\"albumartistslist\"></div>\n");
  vs_string_append (body, "<script src=\"/file/populate_album_artists.js\"></script>\n");

  vs_string_append (body, "<script type=\"text/javascript\">populate_album_artists(");

  if (s_where)
    {
    vs_string_append (body, "\""); 
    vs_string_append (body, s_where); 
    vs_string_append (body, "\""); 
    }
  else
    vs_string_append (body, "\"\""); 

  vs_string_append (body, ",");

  if (s_start)
    vs_string_append (body, s_start); 
  else
    vs_string_append (body, "0"); 

  vs_string_append (body, ",");

  if (s_covers)
    vs_string_append (body, s_covers); 
  else
    vs_string_append (body, "0"); 

  vs_string_append (body, ")</script>\n");

  return body;
  }


