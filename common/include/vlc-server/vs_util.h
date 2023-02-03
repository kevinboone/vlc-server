/*======================================================================
  
  vlc-rest-server

  common/include/vs_util.h

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/
#pragma once

#include <vlc-server/vs_defs.h>

BEGIN_CDECLS

extern const char *vs_util_strerror (VSApiError e);
extern const char *vs_util_strts (VSApiTransportStatus e);

END_CDECLS
