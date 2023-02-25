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

/** Run the scanner. */
extern void scanner_run (const char *media_root, BOOL full, 
         MediaDatabase *mdb, const char *patterns);



