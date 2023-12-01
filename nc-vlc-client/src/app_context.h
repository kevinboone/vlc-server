/*======================================================================
  
  nv-vlc-client 

  nc-vlc-client/src/app_context.h

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/
#pragma once

#include <vlc-server/vs_defs.h>

typedef struct _AppContext
  {
  BOOL local;
  BOOL kiosk;
  BOOL colour;
  int port;
  const char *host;
  } AppContext;


