/*======================================================================
  
  vlc-server

  vs_server_stat.h

  Copyright (c)2022 Kevin Boone, GPL v3.0

  This file contains definitions of all the functions and data
  structures provided by the libvlc-server API library.

======================================================================*/

#include <vlc-server/vs_defs.h>
#include <vlc-server/vs_metadata.h>

/*======================================================================
  
  structures

======================================================================*/

struct _VSServerStat;
typedef struct _VSServerStat VSServerStat;

BEGIN_CDECLS

extern VSServerStat *vs_server_stat_new (VSApiTransportStatus ts,
  const char *mrl, int position, int duration, int index, int volume,
  const char *title, const char *artist, const char *album,
  const char *genre, const char *composer, int scanner_progress);

extern void vs_server_stat_destroy (VSServerStat *self);

VSApiTransportStatus vs_server_stat_get_transport_status 
  (const VSServerStat *self);

/** Get the current medial URL. If there is none, the return value is 
    the empty string (not NULL) */ 
extern const char *vs_server_stat_get_mrl
    (const VSServerStat *self);

/* Get the duration of the stream in msec. If nothing is playing, this
   will usually be 0, but callers should be prepared for it to be
   -1 in conditions of uncertainty. */
extern int vs_server_stat_get_duration (const VSServerStat *self);

/* Get the index of the item in the playlist currently playing. 
   The first item is zero, but clients
   should be prepared for this to be -1 (e.g., if there is no playlist). */ 
extern int vs_server_stat_get_index (const VSServerStat *self);

/* Get the playback position in msec. If nothing is playing, this
   will usually be 0, but callers should be prepared for it to be
   -1 in conditions of uncertainty. */
extern int vs_server_stat_get_position (const VSServerStat *self);

/* Get the volume as a percentage. In error conditions, this might be
   -1. */ 
extern int vs_server_stat_get_volume (const VSServerStat *self);

extern const VSMetadata *vs_server_stat_get_metadata 
      (const VSServerStat *self);

extern int vs_server_stat_get_scanner_progress 
      (const VSServerStat *self);

END_CDECLS

