/*======================================================================
  
  vlc-server

  vs_server_stat.c

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vlc-server/vs_server_stat.h>

/*======================================================================
  
  VSServerStat 

======================================================================*/
struct _VSServerStat
  {
  VSApiTransportStatus transport_status;
  char *mrl;
  int position;
  int duration;
  int index;
  int volume;
  VSMetadata *amd;
  int scanner_progress;
  };

/*======================================================================
  
  vs_server_stat_new

======================================================================*/
VSServerStat *vs_server_stat_new (VSApiTransportStatus ts,
    const char *mrl, int position, int duration, int index, int volume, 
    const char *title, const char *artist, const char *album,
    const char *genre, const char *composer, int scanner_progress)
  {
  VSServerStat *self = malloc (sizeof (VSServerStat));
  self->transport_status = ts;
  if (mrl) 
    self->mrl = strdup (mrl);
  else
    self->mrl = NULL;
  self->position = position;
  self->duration = duration;
  self->index = index;
  self->volume = volume;
  self->amd = vs_metadata_create();
  vs_metadata_set_title (self->amd, title);
  vs_metadata_set_artist (self->amd, artist);
  vs_metadata_set_album (self->amd, album);
  vs_metadata_set_genre (self->amd, genre);
  vs_metadata_set_composer (self->amd, composer);
  self->scanner_progress = scanner_progress;
  return self;
  }

/*======================================================================
  
  vs_server_stat_destroy

======================================================================*/
void vs_server_stat_destroy (VSServerStat *self)
  {
  if (self->mrl) free (self->mrl);
  if (self->amd) free (self->amd);
  free (self);
  }

/*======================================================================
  
  vs_server_stat_get_transport_status 

======================================================================*/
VSApiTransportStatus vs_server_stat_get_transport_status 
    (const VSServerStat *self)
  {
  return self->transport_status;
  }

/*======================================================================
  
  vs_server_stat_get_mrl

======================================================================*/
const char *vs_server_stat_get_mrl
    (const VSServerStat *self)
  {
  return self->mrl;
  }


/*======================================================================
  
  vs_server_stat_get_position

======================================================================*/
int vs_server_stat_get_position (const VSServerStat *self)
  {
  return self->position;
  }

/*======================================================================
  
  vs_server_stat_get_duration

======================================================================*/
int vs_server_stat_get_duration (const VSServerStat *self)
  {
  return self->duration;
  }

/*======================================================================
  
  vs_server_stat_get_index

======================================================================*/
int vs_server_stat_get_index (const VSServerStat *self)
  {
  return self->index;
  }

/*======================================================================
  
  vs_server_stat_get_volume

======================================================================*/
int vs_server_stat_get_volume (const VSServerStat *self)
  {
  return self->volume;
  }

/*======================================================================
  
  vs_server_stat_get_scanner_progress

======================================================================*/
int vs_server_stat_get_scanner_progress (const VSServerStat *self)
  {
  return self->scanner_progress;
  }

/*======================================================================
  
  vs_server_stat_get_metadata

======================================================================*/
const VSMetadata *vs_server_stat_get_metadata 
        (const VSServerStat *self)
  {
  return self->amd;
  }


