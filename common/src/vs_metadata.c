/*============================================================================

  vlc-server

  vs_metadata.c

  Copyright (c)2017 Kevin Boone, GPL v3.0

============================================================================*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <pthread.h>
#include <stdint.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <vlc-server/vs_defs.h>
#include <vlc-server/vs_log.h>
#include <vlc-server/vs_metadata.h>
#include <vlc-server/vs_mimebuffer.h>

struct _VSMetadata
  {
  char *path;
  char *composer;
  char *album;
  char *genre;
  char *artist;
  char *album_artist;
  char *title;
  char *track;
  char *comment;
  char *year;
  time_t mtime;
  size_t size;
  VSMimeBuffer *cover;
  }; 


#define SAFE(x) (x != NULL ? (x) : "")

/*==========================================================================
  vs_metadata_create
  Create a vs_metadata from a copy of the data provided. The caller can, and
    probably should, free the data. This method is safe to call on
    static data, should the need arise.
*==========================================================================*/
VSMetadata *vs_metadata_create ()
  {
  IN
  VSMetadata *self = malloc (sizeof (VSMetadata));
  self->path = NULL;
  self->composer = NULL;
  self->artist = NULL;
  self->album_artist = NULL;
  self->album = NULL;
  self->genre = NULL;
  self->title = NULL;
  self->track = NULL;
  self->comment= NULL;
  self->year = NULL;
  self->cover = NULL;
  self->mtime = 0;
  self->size = 0;
  OUT 
  return self;
  }


/*==========================================================================
  vs_metadata_destroy
*==========================================================================*/
void vs_metadata_destroy (VSMetadata *self)
  {
  IN
  if (self)
    {
    if (self->path) free (self->path);
    if (self->composer) free (self->composer);
    if (self->album) free (self->album);
    if (self->genre) free (self->genre);
    if (self->artist) free (self->artist);
    if (self->album_artist) free (self->album_artist);
    if (self->title) free (self->title);
    if (self->track) free (self->track);
    if (self->comment) free (self->comment);
    if (self->year) free (self->year);
    if (self->cover) vs_mimebuffer_destroy(self->cover);
    free (self);
    }
  OUT
  }

/*==========================================================================

  vs_metadata_get_cover

==========================================================================*/
const VSMimeBuffer *vs_metadata_get_cover (const VSMetadata *self)
  {
  return self->cover;
  }

/*==========================================================================

  vs_metadata_get_composer

==========================================================================*/
const char *vs_metadata_get_composer (const VSMetadata *self)
  {
  return self->composer;
  }

/*==========================================================================

  vs_metadata_get_album

==========================================================================*/
const char *vs_metadata_get_album (const VSMetadata *self)
  {
  return self->album;
  }

/*==========================================================================

  vs_metadata_get_path

==========================================================================*/
const char *vs_metadata_get_path (const VSMetadata *self)
  {
  return self->path;
  }

/*==========================================================================

  vs_metadata_get_genre

==========================================================================*/
const char *vs_metadata_get_genre (const VSMetadata *self)
  {
  return self->genre;
  }

/*==========================================================================

  vs_metadata_get_artist

==========================================================================*/
const char *vs_metadata_get_artist (const VSMetadata *self)
  {
  return self->artist;
  }

/*==========================================================================

  vs_metadata_get_album_artist

==========================================================================*/
const char *vs_metadata_get_album_artist (const VSMetadata *self)
  {
  return self->album_artist;
  }

/*==========================================================================

  vs_metadata_get_title

==========================================================================*/
const char *vs_metadata_get_title (const VSMetadata *self)
  {
  return self->title;
  }

/*==========================================================================

  vs_metadata_get_track

==========================================================================*/
const char *vs_metadata_get_track (const VSMetadata *self)
  {
  return self->track;
  }

/*==========================================================================

  vs_metadata_get_comment

==========================================================================*/
const char *vs_metadata_get_comment (const VSMetadata *self)
  {
  return self->comment;
  }

/*==========================================================================

  vs_metadata_get_year

==========================================================================*/
const char *vs_metadata_get_year (const VSMetadata *self)
  {
  return self->year;
  }

/*==========================================================================

  vs_metadata_get_mtime

==========================================================================*/
time_t vs_metadata_get_mtime (const VSMetadata *self)
  {
  return self->mtime;
  }

/*==========================================================================

  vs_metadata_get_size

==========================================================================*/
size_t vs_metadata_get_size (const VSMetadata *self)
  {
  return self->size;
  }

/*==========================================================================

  replace 

==========================================================================*/
static void replace (char **s1, const char *s2)
  {
  if (*s1) free (*s1);
  if (s2)
    *s1 = strdup (s2);
  else
    *s1 = strdup ("");
  }

/*==========================================================================

  vs_metadata_set_title

==========================================================================*/
void vs_metadata_set_title (VSMetadata *self, const char *title)
  {
  replace (&self->title, title);
  }

/*==========================================================================

  vs_metadata_set_artist

==========================================================================*/
void vs_metadata_set_artist (VSMetadata *self, const char *artist)
  {
  replace (&self->artist, artist);
  }

/*==========================================================================

  vs_metadata_set_album_artist

==========================================================================*/
void vs_metadata_set_album_artist (VSMetadata *self, 
       const char *album_artist)
  {
  replace (&self->album_artist, album_artist);
  }

/*==========================================================================

  vs_metadata_set_composer

==========================================================================*/
void vs_metadata_set_composer (VSMetadata *self, const char *composer)
  {
  replace (&self->composer, composer);
  }


/*==========================================================================

  vs_metadata_set_album

==========================================================================*/
void vs_metadata_set_album (VSMetadata *self, const char *album)
  {
  replace (&self->album, album);
  }

/*==========================================================================

  vs_metadata_set_genre

==========================================================================*/
void vs_metadata_set_genre (VSMetadata *self, const char *genre)
  {
  replace (&self->genre, genre);
  }


/*==========================================================================

  vs_metadata_set_track

==========================================================================*/
void vs_metadata_set_track (VSMetadata *self, const char *track)
  {
  replace (&self->track, track);
  }

/*==========================================================================

  vs_metadata_set_comment

==========================================================================*/
void vs_metadata_set_comment (VSMetadata *self, const char *comment)
  {
  replace (&self->comment, comment);
  }

/*==========================================================================

  vs_metadata_set_year

==========================================================================*/
void vs_metadata_set_year (VSMetadata *self, const char *year)
  {
  replace (&self->year, year);
  }

/*==========================================================================

  vs_metadata_set_path

==========================================================================*/
void vs_metadata_set_path (VSMetadata *self, const char *path)
  {
  replace (&self->path, path);
  }

/*==========================================================================

  vs_metadata_set_size

==========================================================================*/
void vs_metadata_set_size (VSMetadata *self, size_t size)
  {
  self->size = size;
  }

/*==========================================================================

  vs_metadata_set_mtime

==========================================================================*/
void vs_metadata_set_mtime (VSMetadata *self, time_t mtime)
  {
  self->mtime = mtime;
  }

/*==========================================================================

  vs_metadata_set_cover

==========================================================================*/
void vs_metadata_set_cover (VSMetadata *self, VSMimeBuffer *cover)
  {
  if (self->cover) vs_mimebuffer_destroy (self->cover);
  self->cover = cover;
  }

/*==========================================================================

  vs_metadata_dump

==========================================================================*/
void vs_metadata_dump (const VSMetadata *self)
  {
  printf ("path: %s\n", SAFE(self->path));  
  printf ("album: %s\n", SAFE(self->album));  
  printf ("artist: %s\n", SAFE(self->artist));  
  printf ("album artist: %s\n", SAFE(self->album_artist));  
  printf ("comment: %s\n", SAFE(self->comment));  
  printf ("composer: %s\n", SAFE(self->composer));  
  printf ("genre: %s\n", SAFE(self->genre));  
  printf ("title: %s\n", SAFE(self->title));  
  printf ("track: %s\n", SAFE(self->track));  
  printf ("size: %ld\n", self->size);  
  }





