/*============================================================================

  boilerplate 
  buffer.h
  Copyright (c)2020 Kevin Boone, GPL v3.0

============================================================================*/

#pragma once

#include <stdint.h>
#include <vlc-server/vs_defs.h> 

struct _MimeBuffer;
typedef struct _MimeBuffer MimeBuffer;

BEGIN_CDECLS

MimeBuffer *mimebuffer_create (BYTE *data, uint64_t length, 
              const char *mime_type);

MimeBuffer *mimebuffer_create_without_copy (BYTE *data, uint64_t length,
              const char *mime_type);

MimeBuffer *mimebuffer_create_empty (void);

void        mimebuffer_set_contents (MimeBuffer *self, BYTE *data, 
              uint64_t length, const char *mime_type);

void        mimebuffer_destroy (MimeBuffer *self);

uint64_t    mimebuffer_get_length (const MimeBuffer *self);

const BYTE *mimebuffer_get_contents (const MimeBuffer *self);

const char *mimebuffer_get_mime_type (const MimeBuffer *self);

void        mimebuffer_null_terminate (MimeBuffer *self);

BOOL        mimebuffer_write_to_file (const MimeBuffer *self, 
               const char *filename, char **error);

END_CDECLS

