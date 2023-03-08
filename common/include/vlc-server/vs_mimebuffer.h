/*============================================================================

  vlc-server

  vs_vs_mimebuffer.h

  Copyright (c)2020 Kevin Boone, GPL v3.0

============================================================================*/

#pragma once

#include <stdint.h>
#include <vlc-server/vs_defs.h> 

struct _VSMimeBuffer;
typedef struct _VSMimeBuffer VSMimeBuffer;

BEGIN_CDECLS

VSMimeBuffer *vs_mimebuffer_create (BYTE *data, uint64_t length, 
              const char *mime_type);

VSMimeBuffer *vs_mimebuffer_create_without_copy (BYTE *data, uint64_t length,
              const char *mime_type);

VSMimeBuffer *vs_mimebuffer_create_empty (void);

void        vs_mimebuffer_set_contents (VSMimeBuffer *self, BYTE *data, 
              uint64_t length, const char *mime_type);

void        vs_mimebuffer_destroy (VSMimeBuffer *self);

uint64_t    vs_mimebuffer_get_length (const VSMimeBuffer *self);

const BYTE *vs_mimebuffer_get_contents (const VSMimeBuffer *self);

const char *vs_mimebuffer_get_mime_type (const VSMimeBuffer *self);

void        vs_mimebuffer_null_terminate (VSMimeBuffer *self);

BOOL        vs_mimebuffer_write_to_file (const VSMimeBuffer *self, 
               const char *filename, char **error);

END_CDECLS

