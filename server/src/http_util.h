/*======================================================================
  
  vlc-rest-server

  server/src/http_util.h

  Various general-purpose functions for use when processing HTTP
    requests.

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

/** Given a specific 'tag', return the associated data and length. This
      function is used for fetching images and static HTML pages, that
      get linked directly into the server's binary. These files start
      life in the docroot/ directory of the source, but these files are
      not used at runtime. */
extern BOOL http_util_fetch_from_docroot (const char *tag, const BYTE **buff, 
    int *len);

/** Fetch a specific file form the filesystem, returning its data and
      length. */
extern BOOL http_util_fetch_from_filesystem (const char *file, BYTE **buff, 
    int *len);

/** Make a guess about the MIME type from the URL. */
extern const char *http_util_mime_from_path (const char *url);

/** Escape a string for using as an argument to a JavaScript function.
      In particular, ' has to be changed to \'. */ 
extern VSString *http_util_encode_for_js (const char *file);

END_CDECLS



