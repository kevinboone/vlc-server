/*======================================================================
  
  vlc-rest-server

  api/src/util.c

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/
#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include <vlc-server/vs_defs.h>
#include <vlc-server/api-client.h>

/*======================================================================

 libvlc_escape_sql 

======================================================================*/
char *libvlc_escape_sql (const char *sql)
  {
  char *ret;
  if (strchr (sql, '\''))
    {
    int l = strlen (sql);
    int newlen = l;
    ret = malloc (newlen + 1);
    int p = 0;

    for (int i = 0; i < l; i++)
      {
      char c = sql[i];
      if (c == '\'')
        {
        newlen+=2;
        ret = realloc (ret, newlen + 1);
        ret[p++] = '\'';
        ret[p++] = '\'';
        }
      else
        {
        ret[p++] = c;
        }
      }
    ret[p] = 0;
    }
  else
    {
    ret = strdup (sql);
    }
  return ret;
  }

