/*======================================================================
  
  vlc-server

  server/src/gui_streams.c

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
#include "gui_streams.h"
#include "media_database.h"

#define SAFE(x)((x) ? (x) : "")

/*======================================================================
  gui_streams_get_body
======================================================================*/
VSString *gui_streams_get_body (const char *path, 
            const VSProps *arguments, int count, MediaDatabase *mdb)
  {
  (void)path; (void)count;
  VSString *body = vs_string_create ("<h1>Streams</h1>\n");

  if (media_database_is_init (mdb))
    {
    VSSearchConstraints *constraints = vs_search_constraints_new();
    gui_set_constraints_from_arguments (constraints, arguments);
    constraints->start = 0;
    constraints->count = -1;
    VSList *entries = vs_list_create (free);
    char *error = NULL;
    media_database_search_streams (mdb, MDB_COL_STRMNAME, 
       entries, constraints, &error);

    if (error == NULL)
      {
      int l = vs_list_length (entries);
      if (l > 0)
        {
        vs_string_append (body, "<table class=\"resulttable\">\n");
        for (int i = 0; i < l; i++)
	  {
	  const char *entry = vs_list_get (entries, i);
          VSString *enc_name = http_util_encode_for_js (entry); 
          vs_string_append (body, "<tr>");
          vs_string_append (body, "<td>");
          vs_string_append (body, "<a href=\"javascript:cmd_play_stream('");
          vs_string_append (body, vs_string_cstr (enc_name)); 
          vs_string_append (body, "')\">[play]</a> ");
          vs_string_append (body, "</td>");
          vs_string_append (body, "<td>");
          vs_string_append (body, entry);
          vs_string_append (body, "</td>");
          VSMetadata *amd = media_database_get_stream_amd_by_name (mdb, entry);
          if (amd)
            {
            vs_string_append (body, "<td>");
            vs_string_append (body, SAFE(vs_metadata_get_comment (amd)));
            vs_string_append (body, "</td>");
            vs_string_append (body, "<td>");
            vs_string_append (body, SAFE(vs_metadata_get_genre (amd)));
            vs_string_append (body, "</td>");
            vs_metadata_destroy (amd);
            }
          else
            {
            vs_string_append (body, "<td></td>");
            vs_string_append (body, "<td></td>");
            }
          vs_string_append (body, "</tr>");
          vs_string_destroy (enc_name);
          }
	vs_string_append (body, "</table>\n");
        }
      else
        {
        vs_string_append (body, "No streams");
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
    {
    vs_log_error (MDB_ERR_NOT_INIT);
    vs_string_append (body, MDB_ERR_NOT_INIT);
    }
    }

  return body;
  }


