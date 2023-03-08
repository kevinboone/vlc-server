/*============================================================================

  vlc-server 

  server/src/gui.h

  This is the entry point for all functions that construct the web
    use interface. See gui_modules.md for more information.

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

extern void gui_set_constraints_from_arguments 
       (VSSearchConstraints *constraints, const VSProps *arguments);

extern VSString *gui_get_results_page (const char *path, 
            const VSProps *arguments, int count, MediaDatabase *mdb, 
            const char *field_name, const char *header, 
            MediaDatabaseColumn col, 
            BOOL headers, GUICellCallback cell_callback, BOOL tabular);

extern VSString *gui_get_limited_results_page (
            const VSProps *arguments, int count, MediaDatabase *mdb, 
            const char *field_name, const char *header, 
            MediaDatabaseColumn col, 
            BOOL headers, GUICellCallback cell_callback, BOOL tabular);

END_CDECLS


