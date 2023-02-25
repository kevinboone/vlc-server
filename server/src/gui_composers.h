/*============================================================================

  vlc-server 

  server/src/gui_composers.h

  See gui_modules.md for more information.

  Copyright (c)2022 Kevin Boone, GPL v3.0

============================================================================*/

#pragma once

#include <vlc-server/vs_defs.h> 
#include <vlc-server/vs_string.h> 
#include <vlc-server/vs_props.h> 
#include "media_database.h" 

BEGIN_CDECLS

extern void gui_composers_composer_cell (VSString *body, const char *composer, 
       BOOL covers, MediaDatabase *mdb);

extern VSString *gui_composers_get_body (const char *path,
            const VSProps *arguments, BOOL matches_per_page, 
            MediaDatabase *mdb);

END_CDECLS



