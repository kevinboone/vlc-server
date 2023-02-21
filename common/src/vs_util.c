/*============================================================================

  vlc-rest-server 
  vs_util.c
  Copyright (c)2022 Kevin Boone, GPL v3.0

============================================================================*/
#define _GNU_SOURCE

#include "vlc-server/vs_defs.h" 

/*============================================================================

  vs_util_strerror

============================================================================*/
const char *vs_util_strerror (VSApiError e)
  {
  switch (e)
    {
    case VSAPI_ERR_OK:
      return "OK";
    case VSAPI_ERR_NOT_FOUND:
      return "file or path not found";
    case VSAPI_ERR_NO_PLAYLIST:
      return "no playlist";
    case VSAPI_ERR_CONNECT_SERVER:
      return "could not connect to server";
    case VSAPI_ERR_COMMS:
      return "error communicating with server";
    case VSAPI_ERR_FILE_NOT_ALLOWED:
      return "file not allowed";
    case VSAPI_ERR_INDEX_RANGE:
      return "playlist index out of range";
    case VSAPI_ERR_OPEN_DIR:
      return "can't enumerate local directory";
    case VSAPI_ERR_VOL_RANGE:
      return "volume level out of range";
    case VSAPI_ERR_INIT_VLC:
      return "can't initialize the VLC library";
    case VSAPI_ERR_INIT_DB:
      return "media database is not initialized";
    default:
      return "unknown error";
    }
  }

/*============================================================================

  vs_util_strts

============================================================================*/
const char *vs_util_strts (VSApiTransportStatus ts)
  {
  switch (ts)
    {
    case VSAPI_TS_ERROR: return "error";
    case VSAPI_TS_STOPPED: return "stopped";
    case VSAPI_TS_PLAYING: return "playing";
    case VSAPI_TS_PAUSED: return "paused";
    case VSAPI_TS_OPENING: return "opening";
    case VSAPI_TS_BUFFERING: return "buffering";
    case VSAPI_TS_ENDED: return "ended";
    }
  return "unknown"; // Never get here -- switch is exhaustive
  }


