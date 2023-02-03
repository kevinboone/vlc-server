/*======================================================================
  
  vlc-rest-server

  api/src/libvlc_server_stat.c

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vlc-server/libvlc_server_stat.h>

/*======================================================================
  
  LibVlcServerStat 

======================================================================*/
struct _LibVlcServerStat
  {
  VSApiTransportStatus transport_status;
  char *mrl;
  int position;
  int duration;
  int index;
  int volume;
  };

/*======================================================================
  
  libvlc_server_stat_new

======================================================================*/
LibVlcServerStat *libvlc_server_stat_new (VSApiTransportStatus ts,
    const char *mrl, int position, int duration, int index, int volume)
  {
  LibVlcServerStat *self = malloc (sizeof (LibVlcServerStat));
  self->transport_status = ts;
  if (mrl) 
    self->mrl = strdup (mrl);
  else
    self->mrl = NULL;
  self->position = position;
  self->duration = duration;
  self->index = index;
  self->volume = volume;
  return self;
  }

/*======================================================================
  
  libvlc_server_stat_destroy

======================================================================*/
void libvlc_server_stat_destroy (LibVlcServerStat *self)
  {
  if (self->mrl) free (self->mrl);
  free (self);
  }

/*======================================================================
  
  libvlc_server_stat_get_transport_status 

======================================================================*/
VSApiTransportStatus libvlc_server_stat_get_transport_status 
    (const LibVlcServerStat *self)
  {
  return self->transport_status;
  }

/*======================================================================
  
  libvlc_server_stat_get_mrl

======================================================================*/
const char *libvlc_server_stat_get_mrl
    (const LibVlcServerStat *self)
  {
  return self->mrl;
  }


/*======================================================================
  
  libvlc_server_stat_get_position

======================================================================*/
int libvlc_server_stat_get_position (const LibVlcServerStat *self)
  {
  return self->position;
  }

/*======================================================================
  
  libvlc_server_stat_get_duration

======================================================================*/
int libvlc_server_stat_get_duration (const LibVlcServerStat *self)
  {
  return self->duration;
  }

/*======================================================================
  
  libvlc_server_stat_get_index

======================================================================*/
int libvlc_server_stat_get_index (const LibVlcServerStat *self)
  {
  return self->index;
  }

/*======================================================================
  
  libvlc_server_stat_get_volume

======================================================================*/
int libvlc_server_stat_get_volume (const LibVlcServerStat *self)
  {
  return self->volume;
  }

