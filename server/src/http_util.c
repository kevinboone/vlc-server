/*======================================================================
  
  vlc-rest-server

  server/src/http_util.c

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <vlc-server/vs_log.h> 
#include "template_manager.h"
#include "http_util.h"

/*======================================================================

  http_util_fetch_from_docroot

======================================================================*/
BOOL http_util_fetch_from_docroot (const char *url, const BYTE **buff, 
    int *len)
  {
  IN
  vs_log_debug ("Fetch from docroot: %s", url);
  BOOL ret = FALSE;
  ret = template_manager_get_data_by_tag (url, buff, len);
  return ret;
  OUT
  }

/*======================================================================

  http_util_fetch_from_filesystem

======================================================================*/
BOOL http_util_fetch_from_filesystem (const char *file, BYTE **buff, 
    int *len)
  {
  IN
  BOOL ret = FALSE;
  int fd = open (file, O_RDONLY);
  if (fd >= 0)
    {
    struct stat sb; 
    if (fstat (fd, &sb) == 0)
      {
      *len = sb.st_size;
      *buff = malloc (*len);
      read (fd, *buff, *len);
      ret = TRUE;
      }
    close (fd);
    }

  OUT
  return ret;
  }

/*======================================================================

  http_util_mime_from_path

======================================================================*/
const char *http_util_mime_from_path (const char *url)
  {
  IN
  const char *mime = "application/octet-stream";
  char *e = strrchr (url, '.');
  if (e)
    {
    e++;
    if (strcasecmp (e, "jpg") == 0) mime = TYPE_JPEG; 
    else if (strcasecmp (e, "html") == 0) mime = TYPE_HTML; 
    else if (strcasecmp (e, "json") == 0) mime = TYPE_JSON; 
    else if (strcasecmp (e, "js") == 0) mime = TYPE_JAVASCRIPT; 
    else if (strcasecmp (e, "png") == 0) mime = TYPE_PNG; 
    else if (strcasecmp (e, "ico") == 0) mime = TYPE_ICON; 
    }
  OUT
  vs_log_debug ("MIME type for %s is %s", url, mime);
  return mime;
  }


