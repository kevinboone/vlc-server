/*======================================================================
  
  vlc-rest-server

  scanner.h

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/

#pragma once

#include "media_database.h" 

extern void scanner_run (const char *media_root, BOOL full, 
         MediaDatabase *mdb, const char *patterns);



