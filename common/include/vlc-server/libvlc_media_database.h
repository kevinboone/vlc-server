/*============================================================================

  vlc-server 
  common/vlc-server/libvlc_server_media_database.h
  Copyright (c)2022 Kevin Boone, GPL v3.0

============================================================================*/

#pragma once

typedef enum _MediaDatabaseColumn
  {
  MDB_COL_PATH = 0,
  MDB_COL_ALBUM = 1,
  MDB_COL_ARTIST = 2,
  MDB_COL_COMPOSER = 3,
  MDB_COL_GENRE = 4
  } MediaDatabaseColumn;

