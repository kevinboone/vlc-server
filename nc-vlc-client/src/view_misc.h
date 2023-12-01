/*======================================================================
  
  nv-vlc-client 

  nc-vlc-client/src/view_misc.h

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/
#pragma once

#include <vlc-server/vs_defs.h>
#include <vlc-server/api-client.h>
#include "app_context.h" 

typedef void (*VMSelectFunction) 
          (LibVlcServerClient *lvsc, const char *item, 
           const AppContext *context);

extern char *view_misc_fit_string (const char *s, int w);
extern void view_list (WINDOW *main_window, LibVlcServerClient *lvsc, 
         int h, int w, int row, int col, const VSList *list,
         VMSelectFunction select_function, const char *title,
         const AppContext *context);
extern BOOL view_misc_handle_non_menu_key 
         (LibVlcServerClient *lvsc, int ch, const AppContext *context);

