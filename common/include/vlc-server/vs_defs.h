/*======================================================================
  
  vlc-rest-server

  common/include/vs_defs.h

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/
#pragma once

#include <stdint.h>

#ifndef BOOL
typedef int BOOL;
#endif

#ifndef BYTE 
typedef unsigned char BYTE;
#endif

#ifndef UTF8 
typedef unsigned char UTF8;
#endif

#ifndef UTF32 
typedef int32_t UTF32;
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE 
#define FALSE 0
#endif

#ifndef BEGIN_CDECLS
#ifdef __CPLUSPLUS 
#define BEGIN_CDECLS extern "C" {
#else
#define BEGIN_CDECLS 
#endif
#endif

#ifndef END_CDECLS
#ifdef __CPLUSPLUS 
#define END_CDECLS {
#else
#define END_CDECLS 
#endif
#endif

typedef enum _VSApiError
  {
  VSAPI_ERR_OK = 0,
  VSAPI_ERR_NOT_FOUND = 2,
  VSAPI_ERR_NO_PLAYLIST = 3,
  VSAPI_ERR_CONNECT_SERVER = 4,
  // General error in communication with server
  VSAPI_ERR_COMMS = 5,
  VSAPI_ERR_FILE_NOT_ALLOWED = 6,
  // Playlist index out of range
  VSAPI_ERR_INDEX_RANGE = 7,
  // Can't enumerate a local directory
  VSAPI_ERR_OPEN_DIR = 8,
  // Volume out of range 
  VSAPI_ERR_VOL_RANGE = 9, 
  // Can't initialize the VLC library 
  VSAPI_ERR_INIT_VLC = 10,
  // The media database was not initialized
  VSAPI_ERR_INIT_DB = 11,
  // The media database exists, but is completely empty
  VSAPI_ERR_NO_FILES = 12,
  // The media database exists, but has no albums
  VSAPI_ERR_NO_ALBUMS = 13,
  // Some other database-related error (look in log!) 
  VSAPI_ERR_GEN_DB = 14
  } VSApiError;

typedef enum _VSApiTransportStatus
  {
  VSAPI_TS_ERROR = -1, 
  VSAPI_TS_STOPPED = 0,
  VSAPI_TS_PLAYING = 1,
  VSAPI_TS_PAUSED = 2,
  VSAPI_TS_OPENING = 3,
  VSAPI_TS_BUFFERING = 4,
  VSAPI_TS_ENDED = 5
  } VSApiTransportStatus;


