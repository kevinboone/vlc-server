/*======================================================================
  
  nv-vlc-client 

  nc-vlc-client/src/util.h

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/
#pragma once

extern void ms_to_hms (int msec, int *h, int *m, int *s);
extern void ms_to_minsec (int msec, int *m, int *s);
extern char *fit_string (const char *s, int w);


