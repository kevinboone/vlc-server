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
  MDB_COL_GENRE = 4,
  MDB_COL_ALBUM_ARTIST = 5,
  MDB_COL_TITLE = 6,
  MDB_COL_STRMNAME = 7,
  MDB_COL_STRMLOCATION = 8,
  MDB_COL_STRMTAGS = 9,
  MDB_COL_STRMURI = 10 
  } MediaDatabaseColumn;

