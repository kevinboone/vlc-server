/*======================================================================
  
  vlc-rest-server

  server/src/http_server.h

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/

#pragma once

#include <vlc-server/vs_defs.h> 
#include "player.h"

struct _HttpServer;

typedef struct _HttpServer HttpServer;

BEGIN_CDECLS

extern HttpServer *http_server_new (int port, const char *media_root, 
                     int matches_per_page);
extern void        http_server_destroy (HttpServer *self);
extern void        http_server_stop (HttpServer *self);
extern void        http_server_set_player (HttpServer *self, 
                      Player *player);
extern BOOL        http_server_start (HttpServer *self);
extern BOOL        http_server_is_running (const HttpServer *self);

END_CDECLS


