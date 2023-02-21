/*============================================================================

  vlc-server 
  server/src/gui.h
  Copyright (c)2022 Kevin Boone, GPL v3.0

============================================================================*/

#pragma once

#include <vlc-server/vs_defs.h> 
#include <vlc-server/vs_string.h> 
#include <vlc-server/vs_props.h> 
#include "player.h"
#include "media_database.h"

typedef void (*GUICellCallback) (VSString *body, const char *name, 
       BOOL covers, MediaDatabase *mdb);

BEGIN_CDECLS

extern BOOL gui_process_request (Player *player, const char *media_root, 
        const char *url, BYTE **buff, int *len, const VSProps *arguments,
        int matches_per_page, MediaDatabase *mdb, const char *instance_name);

void gui_set_constraints_from_arguments 
       (MediaDatabaseConstraints *constraints, const VSProps *arguments);

VSString *gui_get_results_page (const char *path, 
            const VSProps *arguments, int count, MediaDatabase *mdb, 
            const char *field_name, MediaDatabaseColumn col, 
            BOOL headers, GUICellCallback cell_callback, BOOL tabular);

END_CDECLS


