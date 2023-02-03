/*======================================================================
  
  vlc-rest-server

  server/src/gui_playlist.c

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

/*======================================================================
  
  gui_files_encode_for_js

======================================================================*/
VSString *gui_playlist_encode_for_js (const char *file)
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
  
  gui_playlist_get_body

======================================================================*/
VSString *gui_playlist_get_body (const Player *player, 
            const char *media_root, const char *path, BOOL covers)
  {
  IN
  vs_log_debug ("GUI playlist");
  VSString *body = vs_string_create ("");
  vs_string_append (body, "<h1>Playlist</h1>\n");

  VSList *list = api_playlist (player);
  int l = vs_list_length (list);
  for (int i = 0; i < l; i++)
    {
    const char *item = vs_list_get (list, i);
    vs_string_append_printf (body, "%03d", i + 1);
    vs_string_append (body, " ");
    vs_string_append (body, "<a href=\"javascript:cmd_index(");
    vs_string_append_printf (body, "%d", i);
    vs_string_append (body, ")\">");
    vs_string_append (body, "[play]");
    vs_string_append (body, "</a>");
    vs_string_append (body, " ");
    vs_string_append (body, item);
    vs_string_append (body, "<br/>\n");
    }

  vs_list_destroy (list);

  OUT
  return body;
  }


