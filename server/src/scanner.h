/*======================================================================
  
  vlc-rest-server

  scanner.h

  This module defines the media database scanner. It only exposes 
    one function to callers, but the implementation is actually
    rather complex.

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/

#pragma once

#include "media_database.h" 

#define VLC_SCAN_PROG_FILE "vlc-scanner.progress"

/** Run the scanner. */
extern void scanner_run (const char *media_root, BOOL full, 
         MediaDatabase *mdb, const char *patterns);

/** Get the full pathname of the file that will be used to report
    scanner progress (it will usually be in /tmp) */
extern void scanner_get_progress_file (char *file, int len);

/** Returns the number of files scanned in the current scanner run. If the
    scanner isn't running, returns -1. */
extern int scanner_get_progress (void);

