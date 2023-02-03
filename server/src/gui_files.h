/*============================================================================

  vlc-server 
  server/src/gui_files.h
  Copyright (c)2022 Kevin Boone, GPL v3.0

============================================================================*/

#pragma once

#include <vlc-server/vs_defs.h> 
#include <vlc-server/vs_string.h> 
#include <vlc-server/vs_props.h> 

BEGIN_CDECLS

extern VSString *gui_files_get_body (const Player *player, 
            const char *media_root, const char *path,
            const VSProps *arguments, BOOL matches_per_page);

END_CDECLS



