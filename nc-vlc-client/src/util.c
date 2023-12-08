/*======================================================================
  
  nv-vlc-client 

  nc-vlc-client/src/util.c

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include "util.h" 

/*======================================================================
  
  ms_to_hms 

======================================================================*/
void ms_to_hms (int msec, int *h, int *m, int *s)
  {
  if (msec < 0) msec = 0;
  int sec = msec / 1000;
  *h = sec / 3600;
  sec -= *h * 3600;
  *m = sec / 60;
  sec -= *m * 60;
  *s = sec; 
  }

/*======================================================================
  
  ms_to_minsec

======================================================================*/
void ms_to_minsec (int msec, int *m, int *s)
  {
  if (msec < 0) msec = 0;
  int sec = msec / 1000;
  *m = sec / 60;
  sec -= *m * 60;
  *s = sec; 
  }

/*======================================================================
  
  fit_string

  NOTE this won't work for strings that contain wide characters, and
    it may even corrupt them.

======================================================================*/
char *fit_string (const char *s, int w)
  {
  int l = strlen (s);
  if (l <= w) return strdup (s);
  int remove = l - w; 
  int first = (l - remove) / 2;
  char *ret = strdup (s);
  ret[first] = 0;
  strcat (ret, "...");
  strcat (ret, s + first + remove + 3);
  return ret; 
  }


