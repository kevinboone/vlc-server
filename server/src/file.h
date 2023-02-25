/*============================================================================

  vlc-server 

  server/src/file.h

  This module contains general file-handling funtions. Or, rather, it 
    will -- it's mostly empty at present.

  Copyright (c)2022 Kevin Boone, GPL v3.0

============================================================================*/

#pragma once

#include <vlc-server/vs_defs.h> 

BEGIN_CDECLS

/** Get the audio file extensions that the program will handle by default.
      The format is appropriate for passing to player_new(). */
extern const char *file_get_default_extensions (void);

END_CDECLS


