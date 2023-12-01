/*======================================================================
  
  nv-vlc-client 

  nc-vlc-client/src/status.h

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/
#pragma once

#include <vlc-server/api-client.h>
#include "app_context.h" 

extern void status_update (LibVlcServerClient *lvsc, 
         const AppContext *context);

