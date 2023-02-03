/*============================================================================

  vlc-server 
  
  string.h
  
  Copyright (c)2017 Kevin Boone, GPL v3.0

  ============================================================================*/

#pragma once

#include <stdint.h>
#include <vlc-server/vs_defs.h> 
//#include "list.h"

struct _VSString;
typedef struct _VSString VSString;

BEGIN_CDECLS

VSString      *vs_string_create_empty (void);
VSString      *vs_string_create (const char *s);
/* Create a VSString and absorb a pre-existing buffer, that is safe to
   free when the VSString is destroyed. This method is to save copying
   memory areas when a VSString is created from a previous malloc() or
   strdup () */
VSString      *vs_string_create_and_own (char *s);
VSString      *vs_string_create_with_len (const char *s, int len);
VSString      *vs_string_clone (const VSString *self);
int          vs_string_find (const VSString *self, const char *search);
int          vs_string_find_last (const VSString *self, const char *search);
void         vs_string_destroy (VSString *self);
/* Destroy the VSString object, but leave the underlying memory intact
   (for use by the caller). */
void         vs_string_destroy_wrapper (VSString *self);
const char  *vs_string_cstr (const VSString *self);
const char  *vs_string_cstr_safe (const VSString *self);
void         vs_string_append_printf (VSString *self, const char *fmt,...);
void         vs_string_append (VSString *self, const char *s);
void         vs_string_append_c (VSString *self, const uint32_t c);
void         vs_string_prepend (VSString *self, const char *s);
int          vs_string_length (const VSString *self);
VSString      *vs_string_substitute_all (const VSString *self, 
                const char *search, const char *replace);
void         vs_string_substitute_all_in_place (VSString *self, 
                const char *search, const char *replace);
void        vs_string_delete (VSString *self, const int pos, 
                const int len);
void        vs_string_insert (VSString *self, const int pos, 
                const char *replace);
BOOL        vs_string_create_from_utf8_file (const char *filename, 
                VSString **result, char **error);
VSString     *vs_string_encode_url (const char *s);
void        vs_string_append_byte (VSString *self, const BYTE byte);
void        vs_string_trim_left (VSString *self);
void        vs_string_trim_right (VSString *self);
UTF32      *vs_string_utf8_to_utf32 (const UTF8 *_in);
UTF8       *vs_string_utf32_to_utf8 (const UTF32 *_in);
BOOL        vs_string_ends_with (const VSString *self, const char *test);
int         vs_string_alpha_sort_fn (const void *p1, const void*p2, 
               void *user_data);
/*
List       *vs_string_split (const VSString *self, const char *delim);
List       *vs_string_tokenize (const VSString *self);
*/

END_CDECLS

