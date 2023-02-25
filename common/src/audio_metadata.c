/*============================================================================

  audio-metadata.c

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
#include <vlc-server/audio_metadata.h>
#include <vlc-server/mimebuffer.h>

struct _AudioMetadata
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
  MimeBuffer *cover;
  }; 


#define SAFE(x) (x != NULL ? (x) : "")

/*==========================================================================
  audio_metadata_create
  Create a audio_metadata from a copy of the data provided. The caller can, and
    probably should, free the data. This method is safe to call on
    static data, should the need arise.
*==========================================================================*/
AudioMetadata *audio_metadata_create ()
  {
  IN
  AudioMetadata *self = malloc (sizeof (AudioMetadata));
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
  audio_metadata_destroy
*==========================================================================*/
void audio_metadata_destroy (AudioMetadata *self)
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
    if (self->cover) mimebuffer_destroy(self->cover);
    free (self);
    }
  OUT
  }

/*==========================================================================

  audio_metadata_get_cover

==========================================================================*/
const MimeBuffer *audio_metadata_get_cover (const AudioMetadata *self)
  {
  return self->cover;
  }

/*==========================================================================

  audio_metadata_get_composer

==========================================================================*/
const char *audio_metadata_get_composer (const AudioMetadata *self)
  {
  return self->composer;
  }

/*==========================================================================

  audio_metadata_get_album

==========================================================================*/
const char *audio_metadata_get_album (const AudioMetadata *self)
  {
  return self->album;
  }

/*==========================================================================

  audio_metadata_get_path

==========================================================================*/
const char *audio_metadata_get_path (const AudioMetadata *self)
  {
  return self->path;
  }

/*==========================================================================

  audio_metadata_get_genre

==========================================================================*/
const char *audio_metadata_get_genre (const AudioMetadata *self)
  {
  return self->genre;
  }

/*==========================================================================

  audio_metadata_get_artist

==========================================================================*/
const char *audio_metadata_get_artist (const AudioMetadata *self)
  {
  return self->artist;
  }

/*==========================================================================

  audio_metadata_get_album_artist

==========================================================================*/
const char *audio_metadata_get_album_artist (const AudioMetadata *self)
  {
  return self->album_artist;
  }

/*==========================================================================

  audio_metadata_get_title

==========================================================================*/
const char *audio_metadata_get_title (const AudioMetadata *self)
  {
  return self->title;
  }

/*==========================================================================

  audio_metadata_get_track

==========================================================================*/
const char *audio_metadata_get_track (const AudioMetadata *self)
  {
  return self->track;
  }

/*==========================================================================

  audio_metadata_get_comment

==========================================================================*/
const char *audio_metadata_get_comment (const AudioMetadata *self)
  {
  return self->comment;
  }

/*==========================================================================

  audio_metadata_get_year

==========================================================================*/
const char *audio_metadata_get_year (const AudioMetadata *self)
  {
  return self->year;
  }

/*==========================================================================

  audio_metadata_get_mtime

==========================================================================*/
time_t audio_metadata_get_mtime (const AudioMetadata *self)
  {
  return self->mtime;
  }

/*==========================================================================

  audio_metadata_get_size

==========================================================================*/
size_t audio_metadata_get_size (const AudioMetadata *self)
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

  audio_metadata_set_title

==========================================================================*/
void audio_metadata_set_title (AudioMetadata *self, const char *title)
  {
  replace (&self->title, title);
  }

/*==========================================================================

  audio_metadata_set_artist

==========================================================================*/
void audio_metadata_set_artist (AudioMetadata *self, const char *artist)
  {
  replace (&self->artist, artist);
  }

/*==========================================================================

  audio_metadata_set_album_artist

==========================================================================*/
void audio_metadata_set_album_artist (AudioMetadata *self, 
       const char *album_artist)
  {
  replace (&self->album_artist, album_artist);
  }

/*==========================================================================

  audio_metadata_set_composer

==========================================================================*/
void audio_metadata_set_composer (AudioMetadata *self, const char *composer)
  {
  replace (&self->composer, composer);
  }


/*==========================================================================

  audio_metadata_set_album

==========================================================================*/
void audio_metadata_set_album (AudioMetadata *self, const char *album)
  {
  replace (&self->album, album);
  }

/*==========================================================================

  audio_metadata_set_genre

==========================================================================*/
void audio_metadata_set_genre (AudioMetadata *self, const char *genre)
  {
  replace (&self->genre, genre);
  }


/*==========================================================================

  audio_metadata_set_track

==========================================================================*/
void audio_metadata_set_track (AudioMetadata *self, const char *track)
  {
  replace (&self->track, track);
  }

/*==========================================================================

  audio_metadata_set_comment

==========================================================================*/
void audio_metadata_set_comment (AudioMetadata *self, const char *comment)
  {
  replace (&self->comment, comment);
  }

/*==========================================================================

  audio_metadata_set_year

==========================================================================*/
void audio_metadata_set_year (AudioMetadata *self, const char *year)
  {
  replace (&self->year, year);
  }

/*==========================================================================

  audio_metadata_set_path

==========================================================================*/
void audio_metadata_set_path (AudioMetadata *self, const char *path)
  {
  replace (&self->path, path);
  }

/*==========================================================================

  audio_metadata_set_size

==========================================================================*/
void audio_metadata_set_size (AudioMetadata *self, size_t size)
  {
  self->size = size;
  }

/*==========================================================================

  audio_metadata_set_mtime

==========================================================================*/
void audio_metadata_set_mtime (AudioMetadata *self, time_t mtime)
  {
  self->mtime = mtime;
  }

/*==========================================================================

  audio_metadata_set_cover

==========================================================================*/
void audio_metadata_set_cover (AudioMetadata *self, MimeBuffer *cover)
  {
  if (self->cover) mimebuffer_destroy (self->cover);
  self->cover = cover;
  }

/*==========================================================================

  audio_metadata_dump

==========================================================================*/
void audio_metadata_dump (const AudioMetadata *self)
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





