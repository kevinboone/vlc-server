/*============================================================================

  vlc-server

  vs_vs_mimebuffer.c

  Copyright (c)2017 Kevin Boone, GPL v3.0

  Methods for storing and retrieving blocks of data, with associated
  MIME type. 

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
#include <vlc-server/vs_mimebuffer.h>

struct _VSMimeBuffer
  {
  BYTE *data;
  uint64_t length;
  char *mime_type;
  }; 


/*==========================================================================
  vs_mimebuffer_create
  Create a buffer from a copy of the data provided. The caller can, and
    probably should, free the data. This method is safe to call on
    static data, should the need arise.
*==========================================================================*/
VSMimeBuffer *vs_mimebuffer_create (BYTE *data, uint64_t length, 
     const char *mime_type)
  {
  IN
  VSMimeBuffer *self = vs_mimebuffer_create_empty();
  vs_mimebuffer_set_contents (self, data, length, mime_type);
  OUT 
  return self;
  }

/*==========================================================================
  vs_mimebuffer_create_without_copy
  Create a buffer and own the data supplied. The caller _must_ not free
    this data. This method cannot be called on static data, because
    it will try to free it later
*==========================================================================*/
VSMimeBuffer *vs_mimebuffer_create_without_copy (BYTE *data, uint64_t length,
    const char *mime_type)
  {
  IN
  VSMimeBuffer *self = vs_mimebuffer_create_empty();
  self->data = data; 
  self->length = length; 
  self->mime_type = strdup (mime_type);
  OUT 
  return self;
  }

/*==========================================================================
  vs_mimebuffer_create_empty 
*==========================================================================*/
VSMimeBuffer *vs_mimebuffer_create_empty (void)
  {
  IN
  VSMimeBuffer *self = malloc (sizeof (VSMimeBuffer));
  self->data = NULL;
  self->length = 0;
  self->mime_type = NULL;
  OUT
  return self;
  }


/*==========================================================================
  vs_mimebuffer_set_contents
  COPY and set the contents of the buffer, freeing any previous contents.
  The caller can (and probably should) free its own copy of the data,
    if it was dynamically created. It is safe to call this function on
    static data, although there are probably few good reasons to.
*==========================================================================*/
void vs_mimebuffer_set_contents (VSMimeBuffer *self, BYTE *data, uint64_t length,
       const char *mime_type)
  {
  IN
  if (self->data) free (self->data);
  self->data = malloc (length);
  memcpy (self->data, data, length);
  self->length = length;
  self->mime_type = strdup (mime_type);
  OUT 
  }


/*==========================================================================
  vs_mimebuffer_destroy
*==========================================================================*/
void vs_mimebuffer_destroy (VSMimeBuffer *self)
  {
  IN
  if (self)
    {
    if (self->data) free (self->data);
    if (self->mime_type) free (self->mime_type);
    free (self);
    }
  OUT
  }


/*==========================================================================
  vs_mimebuffer_get_length
*==========================================================================*/
uint64_t vs_mimebuffer_get_length (const VSMimeBuffer *self)
  {
  return self->length;
  }


/*==========================================================================
  vs_mimebuffer_get_contents
*==========================================================================*/
const BYTE *vs_mimebuffer_get_contents (const VSMimeBuffer *self)
  {
  return self->data;
  }


/*==========================================================================
  vs_mimebuffer_get_contents
*==========================================================================*/
const char *vs_mimebuffer_get_mime_type (const VSMimeBuffer *self)
  {
  return self->mime_type;
  }


/*==========================================================================
  vs_mimebuffer_null_terminate
  Write four 0's on the end of the buffer, to make life easier for callers
   that know the data is of the kind that can be manipulated as
   null-terminated, but where there is no guarantee it actually is. Note
   that the length of the string is not affected -- these nulls do not
   count towards the length.

  This whole method is rather ugly, and should only be used for debugging
    purposes.
*==========================================================================*/
void vs_mimebuffer_null_terminate (VSMimeBuffer *self)
  {
  IN
  self->data = realloc (self->data , self->length + 4);
  for (int i = 0; i < 4; i++)
    self->data [self->length + i] = 0;
  OUT
  }


/*==========================================================================
  vs_mimebuffer_write_to_file
*==========================================================================*/
BOOL vs_mimebuffer_write_to_file (const VSMimeBuffer *self, const char *filename,
      char **error)
  {
  IN
  BOOL ret;
  FILE *f = fopen (filename, "w");
  if (f)
    {
    fwrite (self->data, self->length, 1, f);
    ret = TRUE;
    }
  else
    {
    asprintf (error, "Can't write file %s: %s", filename,
       strerror (errno));
    ret = FALSE;
    }
  OUT
  return ret;
  }


