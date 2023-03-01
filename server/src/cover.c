/*============================================================================

  vlc-server 
  server/src/cover.c
  Copyright (c)2022 Kevin Boone, GPL v3.0

============================================================================*/
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <vlc-server/vs_defs.h> 
#include <vlc-server/vs_log.h> 
#include <vlc-server/vs_string.h> 
#include "http_util.h" 

static char *cover_names[] = {"folder", "cover", "Folder", ".folder", NULL};
static char *cover_exts[] = {"jpg", "gif", "png", "jpeg", NULL};

/*============================================================================

  cover_get_cover_path

============================================================================*/
char *cover_get_cover_path (const char *dir)
  {
  IN
  char cover_path[512];
  int dir_len = strlen (dir);
  strcpy (cover_path, dir);
  int i = 0;
  char *cover_name = cover_names[i];
  char *ret = NULL;
  do
    {
    cover_path[dir_len] = '/';
    cover_path[dir_len + 1] = 0;
    strcat (cover_path + dir_len + 1, cover_name);

    int j = 0;
    char *cover_ext = cover_exts[j];
    int dir_len2 = strlen (cover_path);
    do
      {
      cover_path[dir_len2] = 0;
      strcat (cover_path + dir_len2, "."); 
      strcat (cover_path + dir_len2, cover_ext);
      if (access (cover_path, R_OK) == 0)
        {
        ret = cover_path;
        }
      j++;
      cover_ext = cover_exts[j];
      } while (cover_ext && !ret);

    i++;
    cover_name = cover_names[i];
    } while (cover_name && !ret);
  OUT
  if (ret) return strdup (ret);
  return NULL;
  }

/*============================================================================

  cover_process_request

============================================================================*/
BOOL cover_process_request (const char *media_root, 
        const char *url, BYTE **buff, int *len, const char **type)
  {
  IN

  BOOL ret = FALSE;
  if (url[0] == '=')
    {
    const BYTE *_buff;
    ret = http_util_fetch_from_docroot ("stream_cover.png", &_buff, len);
    *buff = malloc (*len);
    memcpy (*buff, _buff, *len);
    *type = TYPE_PNG; 
    }
  else
    {
    char *dir;
    asprintf (&dir, "%s/%s", media_root, url);

    char *cover_path = cover_get_cover_path (dir);
    if (cover_path)
      {
      ret = http_util_fetch_from_filesystem (cover_path, buff, len);
      *type = http_util_mime_from_path (cover_path);

      free (cover_path);
      }
    else
      {
      const BYTE *_buff;
      ret = http_util_fetch_from_docroot ("default_cover.png", &_buff, len);
      *buff = malloc (*len);
      memcpy (*buff, _buff, *len);
      *type = TYPE_PNG; 
      }

    free (dir);
    }
  OUT
  return ret;
  }

