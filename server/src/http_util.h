/*======================================================================
  
  vlc-rest-server

  server/src/http_util.h

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/

#pragma once

#include <vlc-server/vs_defs.h> 
#include "player.h"

#define TYPE_TEXT "text/plain; charset=utf8"
#define TYPE_HTML "text/html; charset=utf8"
#define TYPE_JSON "application/json; charset=utf8"
#define TYPE_JPEG "image/jpeg"
#define TYPE_PNG "image/png"
#define TYPE_JAVASCRIPT "application/javascript"
#define TYPE_ICON "image/x-icon"

struct _HttpServer;

typedef struct _HttpServer HttpServer;

BEGIN_CDECLS

extern BOOL http_util_fetch_from_docroot (const char *tag, const BYTE **buff, 
    int *len);
extern BOOL http_util_fetch_from_filesystem (const char *file, BYTE **buff, 
    int *len);
extern const char *http_util_mime_from_path (const char *url);

END_CDECLS



