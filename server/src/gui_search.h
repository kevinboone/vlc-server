/*============================================================================

  vlc-server 

  server/src/gui_search.h

  See gui_modules.md for more information.

  Copyright (c)2022 Kevin Boone, GPL v3.0

============================================================================*/

#pragma once

#include <vlc-server/vs_defs.h> 
#include <vlc-server/vs_string.h> 

BEGIN_CDECLS

extern VSString *gui_search_get_body (MediaDatabase *mdb, 
                   const VSProps *arguments, int matches_per_page);

END_CDECLS



