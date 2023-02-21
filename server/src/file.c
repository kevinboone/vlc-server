/*======================================================================
  
  vlc-server

  server/src/file.c

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/

#define _GNU_SOURCE
#include <vlc-server/vs_defs.h> 
#include "file.h"

/*======================================================================
  file_get_default_extensions
======================================================================*/
const char *file_get_default_extensions (void)
 {
 return "*.flac,*.mp3,*.m4a,*.aac,*.ogg";
 }

