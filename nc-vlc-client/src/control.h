/*======================================================================
  
  nv-vlc-client 

  nc-vlc-client/src/control.h

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/
#pragma once

#include <vlc-server/vs_defs.h>
#include <vlc-server/api-client.h>
#include "view_misc.h" 

extern void control_next (LibVlcServerClient *lvsc, 
              const AppContext *context);
extern void control_prev (LibVlcServerClient *lvsc, 
              const AppContext *context);
extern void control_stop (LibVlcServerClient *lvsc, 
              const AppContext *context);
extern void control_volume_down (LibVlcServerClient *lvsc, 
              const AppContext *context);
extern void control_volume_up (LibVlcServerClient *lvsc, 
              const AppContext *context);
extern void control_toggle_pause (LibVlcServerClient *lvsc, 
              const AppContext *context);
extern void control_run_scanner (LibVlcServerClient *lvsc, 
              const AppContext *context);
extern void control_play_random_album (LibVlcServerClient *lvsc, 
              const AppContext *context);


