/*======================================================================
  
  nv-vlc-client 

  nc-vlc-client/src/view_genres.h

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/
#pragma once

#include <vlc-server/api-client.h>

extern void view_genres (WINDOW *main_window, LibVlcServerClient *lvsc, 
       int h, int w, int row, int col, const VMContext *context);

