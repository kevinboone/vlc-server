/*======================================================================
  
  nv-vlc-client 

  nc-vlc-client/src/info_window.h

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/
#pragma once

#include <vlc-server/api-client.h>
#include "app_context.h" 

void info_window_run (WINDOW *main_window, LibVlcServerClient *lvsc,
       const AppContext *context);

