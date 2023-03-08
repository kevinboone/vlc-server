/*======================================================================
  
  nv-vlc-client 

  nc-vlc-client/src/view_misc.h

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/
#pragma once

#include <vlc-server/vs_defs.h>
#include <vlc-server/api-client.h>

typedef void (*VMSelectFunction) 
          (LibVlcServerClient *lvsc, const char *item);

extern void view_misc_next (LibVlcServerClient *lvsc);
extern void view_misc_prev (LibVlcServerClient *lvsc);
extern void view_misc_stop (LibVlcServerClient *lvsc);
extern void view_misc_volume_down (LibVlcServerClient *lvsc);
extern void view_misc_volume_up (LibVlcServerClient *lvsc);
extern void view_misc_toggle_pause (LibVlcServerClient *lvsc);
extern char *view_misc_fit_string (const char *s, int w);
extern void view_list (WINDOW *main_window, LibVlcServerClient *lvsc, 
         int h, int w, int row, int col, const VSList *list,
         VMSelectFunction select_function, const char *title,
         BOOL kiosk);
extern void view_misc_run_scanner (LibVlcServerClient *lvsc);

