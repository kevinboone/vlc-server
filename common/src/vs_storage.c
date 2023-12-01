/*======================================================================
  
  vlc-server

  vs_storage.c

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vlc-server/vs_storage.h>

/*======================================================================
  
  VSStorage 

======================================================================*/
struct _VSStorage
  {
  int capacity_mb;
  int free_mb;
  int albums;
  int tracks;
  };

/*======================================================================
  
  vs_storage_new

======================================================================*/
VSStorage *vs_storage_new (int capacity_mb, int free_mb, int albums,
    int tracks)
  {
  VSStorage *self = malloc (sizeof (VSStorage));
  self->capacity_mb = capacity_mb;
  self->free_mb = free_mb;
  self->albums = albums;
  self->tracks = tracks;
  return self;
  }

/*======================================================================
  
  vs_storage_destroy

======================================================================*/
void vs_storage_destroy (VSStorage *self)
  {
  free (self);
  }

/*======================================================================
  
  vs_storage_get_capacity_mb

======================================================================*/
extern int vs_storage_get_capacity_mb (const VSStorage *self)
  {
  return self->capacity_mb;
  }

/*======================================================================
  
  vs_storage_get_free_mb

======================================================================*/
extern int vs_storage_get_free_mb (const VSStorage *self)
  {
  return self->free_mb;
  }


/*======================================================================
  
  vs_storage_get_albums

======================================================================*/
extern int vs_storage_get_albums (const VSStorage *self)
  {
  return self->albums;
  }

/*======================================================================
  
  vs_storage_get_tracks

======================================================================*/
extern int vs_storage_get_tracks (const VSStorage *self)
  {
  return self->tracks;
  }


