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

BEGIN_CDECLS

extern BOOL gui_process_request (Player *player, const char *media_root, 
        const char *url, BYTE **buff, int *len, const VSProps *arguments,
        int matches_per_page);

END_CDECLS


