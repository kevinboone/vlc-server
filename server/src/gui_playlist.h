/*============================================================================

  vlc-server 
  server/src/gui_playlist.h
  Copyright (c)2022 Kevin Boone, GPL v3.0

============================================================================*/

#pragma once

#include <vlc-server/vs_defs.h> 
#include <vlc-server/vs_string.h> 

BEGIN_CDECLS

extern VSString *gui_playlist_get_body (const Player *player, 
            const char *media_root);

END_CDECLS



