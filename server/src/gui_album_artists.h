/*============================================================================

  vlc-server 

  server/src/gui_album_artists.h

  See gui_modules.md for more information.

  Copyright (c)2022 Kevin Boone, GPL v3.0

============================================================================*/

#pragma once

#include <vlc-server/vs_defs.h> 
#include <vlc-server/vs_string.h> 
#include <vlc-server/vs_props.h> 
#include "media_database.h" 

BEGIN_CDECLS

extern void gui_album_artists_artist_cell (VSString *body, 
       const char *album_artist, BOOL covers, MediaDatabase *mdb);

extern VSString *gui_album_artists_get_body (const char *path,
            const VSProps *arguments, BOOL matches_per_page, 
            MediaDatabase *mdb);

END_CDECLS



