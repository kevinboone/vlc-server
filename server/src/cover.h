/*============================================================================

  vlc-server 
  server/src/cover.h
  Copyright (c)2022 Kevin Boone, GPL v3.0

============================================================================*/

#pragma once

#include <vlc-server/vs_defs.h> 
#include <vlc-server/vs_string.h> 

BEGIN_CDECLS

extern BOOL cover_process_request (const char *media_root, 
        const char *url, BYTE **buff, int *len, const char **type);

END_CDECLS


