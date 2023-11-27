/*======================================================================
  
  nv-vlc-client 

  nc-vlc-client/src/view_misc.h

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/
#pragma once

#include <vlc-server/vs_defs.h>
#include <vlc-server/api-client.h>

typedef struct _VMContext
  {
  BOOL local;
  BOOL kiosk;
  int port;
  const char *host;
  } VMContext;

typedef void (*VMSelectFunction) 
          (LibVlcServerClient *lvsc, const char *item, 
           const VMContext *context);

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
         const VMContext *context);
extern void view_misc_run_scanner (LibVlcServerClient *lvsc);
extern BOOL view_misc_handle_non_menu_key 
         (LibVlcServerClient *lvsc, int ch);

