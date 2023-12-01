/*======================================================================
  
  nv-vlc-client 

  nc-vlc-client/src/view_albums.h

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/
#pragma once

#include <vlc-server/api-client.h>
#include "app_context.h" 

extern void view_albums (WINDOW *main_window, LibVlcServerClient *lvsc, 
       int h, int w, int row, int col, const char *caption_prefix, 
       const char *where, const AppContext *context);

