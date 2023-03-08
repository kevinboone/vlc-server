/*============================================================================

  vs_metadata.h

  Copyright (c)202a-30 Kevin Boone, GPL v3.0

============================================================================*/

#pragma once

#include <stdint.h>
#include <vlc-server/vs_defs.h>
#include <vlc-server/vs_mimebuffer.h>

struct _VSMetadata;
typedef struct _VSMetadata VSMetadata;

BEGIN_CDECLS

extern VSMetadata    *vs_metadata_create (void);

extern void              vs_metadata_destroy (VSMetadata *self);

extern const char       *vs_metadata_get_path (const VSMetadata *self);
extern const char       *vs_metadata_get_title (const VSMetadata *self);
extern const char       *vs_metadata_get_artist (const VSMetadata *self);
extern const char       *vs_metadata_get_album_artist (const VSMetadata *self);
extern const char       *vs_metadata_get_composer (const VSMetadata *self);
extern const char       *vs_metadata_get_album (const VSMetadata *self);
extern const char       *vs_metadata_get_genre (const VSMetadata *self);
extern const char       *vs_metadata_get_track (const VSMetadata *self);
extern const char       *vs_metadata_get_comment (const VSMetadata *self);
extern const char       *vs_metadata_get_year (const VSMetadata *self);
extern size_t            vs_metadata_get_size (const VSMetadata *self);
extern time_t            vs_metadata_get_mtime (const VSMetadata *self);
extern const VSMimeBuffer *vs_metadata_get_cover (const VSMetadata *self);

extern void vs_metadata_set_path (VSMetadata *self, const char *path);
extern void vs_metadata_set_title (VSMetadata *self, const char *title);
extern void vs_metadata_set_artist (VSMetadata *self, const char *artist);
extern void vs_metadata_set_album_artist (VSMetadata *self, 
                            const char *album_artist);
extern void vs_metadata_set_composer (VSMetadata *self, 
                            const char *composer);
extern void vs_metadata_set_album (VSMetadata *self, const char *album);
extern void vs_metadata_set_genre (VSMetadata *self, const char *genre);
extern void vs_metadata_set_track (VSMetadata *self, const char *track);
extern void vs_metadata_set_comment (VSMetadata *self, 
                            const char *comment);
extern void vs_metadata_set_year (VSMetadata *self, const char *year);
extern void vs_metadata_set_size (VSMetadata *self, size_t size);
extern void vs_metadata_set_mtime (VSMetadata *self, time_t mtime);
// set_cover takes ownership of the cover VSMimeBuffer, which should not be freed
extern void vs_metadata_set_cover (VSMetadata *self, VSMimeBuffer *cover);

extern void vs_metadata_dump (const VSMetadata *self);

END_CDECLS

