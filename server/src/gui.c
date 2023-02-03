/*======================================================================
  
  vlc-rest-server

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

/*======================================================================
  
  gui_process_request

======================================================================*/
BOOL gui_process_request (Player *player, const char *media_root, 
      const char *url, BYTE **buff, int *len, const VSProps *arguments,
      int matches_per_page)
  {
  IN
  BOOL ret = FALSE;

  VSString *s = template_manager_get_string_by_tag ("generic.html"); 
  if (s)
    {
    if (strncmp (url, "files/", 5) == 0)
      {
      VSString *body = gui_files_get_body (player, media_root, url + 5,
        arguments, matches_per_page);
      template_manager_substitute_placeholder 
          (s, "BODY", vs_string_cstr (body));
      template_manager_substitute_placeholder 
          (s, "TITLE", NAME); // TODO
      vs_string_destroy (body);
      }
    else if (strncmp (url, "playlist", 8) == 0)
      {
      VSString *body = gui_playlist_get_body (player, media_root);
      template_manager_substitute_placeholder 
          (s, "BODY", vs_string_cstr (body));
      template_manager_substitute_placeholder 
          (s, "TITLE", NAME); // TODO
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


