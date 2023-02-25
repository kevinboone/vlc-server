/*============================================================================

  audio_metadata.h

  Copyright (c)202a-30 Kevin Boone, GPL v3.0

============================================================================*/

#pragma once

#include <stdint.h>
#include <vlc-server/vs_defs.h>
#include <vlc-server/mimebuffer.h>

struct _AudioMetadata;
typedef struct _AudioMetadata AudioMetadata;

BEGIN_CDECLS

extern AudioMetadata    *audio_metadata_create (void);

extern void              audio_metadata_destroy (AudioMetadata *self);

extern const char       *audio_metadata_get_path (const AudioMetadata *self);
extern const char       *audio_metadata_get_title (const AudioMetadata *self);
extern const char       *audio_metadata_get_artist (const AudioMetadata *self);
extern const char       *audio_metadata_get_album_artist (const AudioMetadata *self);
extern const char       *audio_metadata_get_composer (const AudioMetadata *self);
extern const char       *audio_metadata_get_album (const AudioMetadata *self);
extern const char       *audio_metadata_get_genre (const AudioMetadata *self);
extern const char       *audio_metadata_get_track (const AudioMetadata *self);
extern const char       *audio_metadata_get_comment (const AudioMetadata *self);
extern const char       *audio_metadata_get_year (const AudioMetadata *self);
extern size_t            audio_metadata_get_size (const AudioMetadata *self);
extern time_t            audio_metadata_get_mtime (const AudioMetadata *self);
extern const MimeBuffer *audio_metadata_get_cover (const AudioMetadata *self);

extern void audio_metadata_set_path (AudioMetadata *self, const char *path);
extern void audio_metadata_set_title (AudioMetadata *self, const char *title);
extern void audio_metadata_set_artist (AudioMetadata *self, const char *artist);
extern void audio_metadata_set_album_artist (AudioMetadata *self, 
                            const char *album_artist);
extern void audio_metadata_set_composer (AudioMetadata *self, 
                            const char *composer);
extern void audio_metadata_set_album (AudioMetadata *self, const char *album);
extern void audio_metadata_set_genre (AudioMetadata *self, const char *genre);
extern void audio_metadata_set_track (AudioMetadata *self, const char *track);
extern void audio_metadata_set_comment (AudioMetadata *self, 
                            const char *comment);
extern void audio_metadata_set_year (AudioMetadata *self, const char *year);
extern void audio_metadata_set_size (AudioMetadata *self, size_t size);
extern void audio_metadata_set_mtime (AudioMetadata *self, time_t mtime);
// set_cover takes ownership of the cover MimeBuffer, which should not be freed
extern void audio_metadata_set_cover (AudioMetadata *self, MimeBuffer *cover);

extern void audio_metadata_dump (const AudioMetadata *self);

END_CDECLS

