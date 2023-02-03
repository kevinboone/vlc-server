/*======================================================================
  
  vlc-rest-server

  api-client.h

  Copyright (c)2022 Kevin Boone, GPL v3.0

  This file contains definitions of all the functions and data
  structures provided by the libvlc-server API library.

======================================================================*/

#include <vlc-server/vs_defs.h>

/*======================================================================
  
  structures

======================================================================*/

struct _LibVlcServerStat;
typedef struct _LibVlcServerStat LibVlcServerStat;

BEGIN_CDECLS

extern LibVlcServerStat *libvlc_server_stat_new (VSApiTransportStatus ts,
  const char *mrl, int position, int duration, int index, int volume);

extern void libvlc_server_stat_destroy (LibVlcServerStat *self);
VSApiTransportStatus libvlc_server_stat_get_transport_status 
  (const LibVlcServerStat *self);

/** Get the current medial URL. If there is none, the return value is 
    the empty string (not NULL) */ 
extern const char *libvlc_server_stat_get_mrl
    (const LibVlcServerStat *self);

/* Get the duration of the stream in msec. If nothing is playing, this
   will usually be 0, but callers should be prepared for it to be
   -1 in conditions of uncertainty. */
extern int libvlc_server_stat_get_duration (const LibVlcServerStat *self);

/* Get the index of the item in the playlist currently playing. 
   The first item is zero, but clients
   should be prepared for this to be -1 (e.g., if there is no playlist). */ 
extern int libvlc_server_stat_get_index (const LibVlcServerStat *self);

/* Get the playback position in msec. If nothing is playing, this
   will usually be 0, but callers should be prepared for it to be
   -1 in conditions of uncertainty. */
extern int libvlc_server_stat_get_position (const LibVlcServerStat *self);

/* Get the volume as a percentage. In error conditions, this might be
   -1. */ 
extern int libvlc_server_stat_get_volume (const LibVlcServerStat *self);

END_CDECLS

