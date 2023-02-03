/*============================================================================

  vlc-rest-server 
  string.c
  Copyright (c)2017-2022 Kevin Boone, GPL v3.0

  Methods for handling ASCII/UTF-8 strings. Be aware that these methods
  are just thin wrappers around standard, old-fashioned C library functions,
  and some will misbehave if the string actually contains multi-byte
  characters. In particular, the length() method returns the number of
  bytes, not the number of characters. Methods that search the string may
  potentially match the second or later byte of a multi-byte character.
  Any use of these methods for handling 'real' multibyte UTF-8 needs to
  be tested very carefully.

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
#include <vlc-server/vs_string.h>
#include "convertutf.h" 
//#include "wstring.h" 
//#include "file.h" 
//#include "path.h" 
//#include "list.h" 

struct _VSString
  {
  char *str;
  }; 


/*==========================================================================
vs_string_create_empty 
*==========================================================================*/
VSString *vs_string_create_empty (void)
  {
  return vs_string_create ("");
  }


/*==========================================================================
vs_string_create
*==========================================================================*/
VSString *vs_string_create (const char *s)
  {
  VSString *self = malloc (sizeof (VSString));
  self->str = strdup (s);
  return self;
  }

/*==========================================================================
vs_string_create_and_own
*==========================================================================*/
VSString *vs_string_create_and_own (char *s)
  {
  VSString *self = malloc (sizeof (VSString));
  self->str = s;
  return self;
  }


/*==========================================================================
vs_string_create_with_len
*==========================================================================*/
VSString *vs_string_create_with_len (const char *s, int len)
  {
  VSString *self = malloc (sizeof (VSString));
  self->str = malloc (len + 1);
  memcpy (self->str, s, len);
  self->str [len] = 0;
  return self;
  }


/*==========================================================================
vs_string_destroy
*==========================================================================*/
void vs_string_destroy (VSString *self)
  {
  if (self)
    {
    if (self->str) free (self->str);
    free (self);
    }
  }

/*==========================================================================
vs_string_destroy_wrapper
*==========================================================================*/
void vs_string_destroy_wrapper (VSString *self)
  {
  if (self)
    {
    // Leave self->str alone, if it is allocated
    free (self);
    }
  }


/*==========================================================================
vs_string_cstr
*==========================================================================*/
const char *vs_string_cstr (const VSString *self)
  {
  return self->str;
  }


/*==========================================================================
vs_string_cstr_safe
*==========================================================================*/
const char *vs_string_cstr_safe (const VSString *self)
  {
  if (self)
    {
    if (self->str) 
      return self->str;
    else
      return "";
    }
  else
    return "";
  }


/*==========================================================================
vs_string_append
*==========================================================================*/
void vs_string_append (VSString *self, const char *s) 
  {
  if (!s) return;
  if (self->str == NULL) self->str = strdup ("");
  int newlen = strlen (self->str) + strlen (s) + 2;
  self->str = realloc (self->str, newlen);
  strcat (self->str, s);
  }


/*==========================================================================
vs_string_prepend
*==========================================================================*/
void vs_string_prepend (VSString *self, const char *s) 
  {
  if (!s) return;
  if (self->str == NULL) self->str = strdup ("");
  int newlen = strlen (self->str) + strlen (s) + 2;
  char *temp = strdup (self->str); 
  free (self->str);
  self->str = malloc (newlen);
  strcpy (self->str, s);
  strcat (self->str, temp);
  free (temp);
  }


/*==========================================================================
vs_string_append_printf
*==========================================================================*/
void vs_string_append_printf (VSString *self, const char *fmt,...) 
  {
  if (self->str == NULL) self->str = strdup ("");
  va_list ap;
  va_start (ap, fmt);
  char *s;
  vasprintf (&s, fmt, ap);
  vs_string_append (self, s);
  free (s);
  va_end (ap);
  }


/*==========================================================================
vs_string_length
*==========================================================================*/
int vs_string_length (const VSString *self)
  {
  if (self == NULL) return 0;
  if (self->str == NULL) return 0;
  return strlen (self->str);
  }


/*==========================================================================
vs_string_clone
*==========================================================================*/
VSString *vs_string_clone (const VSString *self)
  {
  if (!self->str) return vs_string_create_empty();
  return vs_string_create (vs_string_cstr (self));
  }


/*==========================================================================
vs_string_find
*==========================================================================*/
int vs_string_find (const VSString *self, const char *search)
  {
  const char *p = strstr (self->str, search);
  if (p)
    return p - self->str;
  else
    return -1;
  }


/*==========================================================================
vs_string_find_last
*==========================================================================*/
int vs_string_find_last (const VSString *self, const char *search)
  {
  int lsearch = strlen (search); 
  int lself = strlen (self->str);
  if (lsearch > lself) return -1; // Can't find a long string in short one
  for (int i = lself - lsearch; i >= 0; i--)
    {
    BOOL diff = FALSE;
    for (int j = 0; j < lsearch && !diff; j++)
      {
      if (search[j] != self->str[i + j]) diff = TRUE;
      }
    if (!diff) return i;
    }
  return -1;
  }


/*==========================================================================
vs_string_delete
*==========================================================================*/
void vs_string_delete (VSString *self, const int pos, const int len)
  {
  char *str = self->str;
  if (pos + len > strlen (str))
    vs_string_delete (self, pos, strlen(str) - len);
  else
    {
    char *buff = malloc (strlen (str) - len + 2);
    strncpy (buff, str, pos); 
    strcpy (buff + pos, str + pos + len);
    free (self->str);
    self->str = buff;
    }
  }


/*==========================================================================
vs_string_insert
*==========================================================================*/
void vs_string_insert (VSString *self, const int pos, 
    const char *replace)
  {
  char *buff = malloc (strlen (self->str) + strlen (replace) + 2);
  char *str = self->str;
  strncpy (buff, str, pos);
  buff[pos] = 0;
  strcat (buff, replace);
  strcat (buff, str + pos); 
  free (self->str);
  self->str = buff;
  }

/*==========================================================================
vs_string_substitute_all_in_place
*==========================================================================*/
void vs_string_substitute_all_in_place (VSString *self, 
    const char *search, const char *replace)
  {
  VSString *n = vs_string_substitute_all (self, search, replace);
  char *ns = strdup (n->str);
  char *temp = self->str;  
  self->str = ns;
  free (temp);
  vs_string_destroy (n);
  }

/*==========================================================================
vs_string_substitute_all
*==========================================================================*/
VSString *vs_string_substitute_all (const VSString *self, 
    const char *search, const char *replace)
  {
  const char *gibberish = "#@x!>Aa;";
  VSString *working = vs_string_clone (self);
  BOOL cont = TRUE;
  while (cont)
    {
    int i = vs_string_find (working, search);
    if (i >= 0)
      {
      vs_string_delete (working, i, strlen (search));
      vs_string_insert (working, i, gibberish);
      }
    else
      cont = FALSE;
    }
  cont = TRUE;
  while (cont)
    {
    int i = vs_string_find (working, gibberish);
    if (i >= 0)
      {
      vs_string_delete (working, i, strlen (gibberish));
      vs_string_insert (working, i, replace);
      }
    else
      cont = FALSE;
    }
  return working;
  }


/*==========================================================================
  vs_string_create_from_utf8_file 
*==========================================================================*/
BOOL vs_string_create_from_utf8_file (const char *filename, 
    VSString **result, char **error)
  {
  VSString *self = NULL;
  BOOL ok = FALSE; 
  int f = open (filename, O_RDONLY);
  if (f > 0)
    {
    self = malloc (sizeof (VSString));
    struct stat sb;
    fstat (f, &sb);
    int64_t size = sb.st_size;
    char *buff = malloc (size + 2);
    read (f, buff, size);
    self->str = buff; 
    self->str[size] = 0;
    *result = self;
    ok = TRUE;
    }
  else
    {
    asprintf (error, "Can't open file '%s' for reading: %s", 
      filename, strerror (errno));
    ok = FALSE;
    }

  return ok;
  }


/*==========================================================================
  vs_string_encode_url
*==========================================================================*/
static char to_hex(char code)
  {
  static char hex[] = "0123456789abcdef";
  return hex[code & 15];
  }


VSString *vs_string_encode_url (const char *str)
  {
  if (!str) return vs_string_create_empty();;
  const char *pstr = str; 
  char *buf = malloc(strlen(str) * 3 + 1), *pbuf = buf;
  while (*pstr)
    {
    if (isalnum(*pstr) || *pstr == '-' || *pstr == '_'
      || *pstr == '.' || *pstr == '~')
      *pbuf++ = *pstr;
    //else if (*pstr == ' ')
    //  *pbuf++ = '+';
    else
      *pbuf++ = '%', *pbuf++ = to_hex(*pstr >> 4),
         *pbuf++ = to_hex(*pstr & 15);
    pstr++;
    }
  *pbuf = '\0';
  VSString *result = vs_string_create (buf);
  free (buf);
  return (result);
  }

/*==========================================================================
  vs_string_append_byte
*==========================================================================*/
void vs_string_append_byte (VSString *self, const BYTE byte)
  {
  char buff[2];
  buff[0] = byte;
  buff[1] = 0;
  vs_string_append (self, buff);
  }


/*==========================================================================
  vs_string_append_c
*==========================================================================*/
void vs_string_append_c (VSString *self, const uint32_t ch)
  {
  if (ch < 0x80) 
    {
    vs_string_append_byte (self, (BYTE)ch);
    }
  else if (ch < 0x0800) 
    {
    vs_string_append_byte (self, (BYTE)((ch >> 6) | 0xC0));
    vs_string_append_byte (self, (BYTE)((ch & 0x3F) | 0x80));
    }
  else if (ch < 0x10000) 
    {
    vs_string_append_byte (self, (BYTE)((ch >> 12) | 0xE0));
    vs_string_append_byte (self, (BYTE)((ch >> 6 & 0x3F) | 0x80));
    vs_string_append_byte (self, (BYTE)((ch & 0x3F) | 0x80));
    }
  else 
    {
    vs_string_append_byte (self, (BYTE)((ch >> 18) | 0xF0));
    vs_string_append_byte (self, (BYTE)(((ch >> 12) & 0x3F) | 0x80));
    vs_string_append_byte (self, (BYTE)(((ch >> 6) & 0x3F) | 0x80));
    vs_string_append_byte (self, (BYTE)((ch & 0x3F) | 0x80));
    }
  }


/*==========================================================================
  vs_string_trim_left
*==========================================================================*/
void vs_string_trim_left (VSString *self)
  {
  const char *s = self->str;
  int l = strlen (s);
  int i = 0;
  int pos = 0;
  BOOL stop = FALSE;
  while (i < l && !stop)
    {
    char c = s[i];
    if (c == ' ' || c == '\n' || c == '\t')
      {
      pos++;
      }
    else
      stop = TRUE;
    i++;
    }
  char *s_new = strdup (s + pos);
  free (self->str);
  self->str = s_new;
  }


/*==========================================================================
  vs_string_trim_right
*==========================================================================*/
void vs_string_trim_right (VSString *self)
  {
  char *s = self->str;
  int l = strlen (s);
  int i = l - 1;
  BOOL stop = FALSE;
  while (i >= 0 && !stop)
    {
    char c = s[i];
    if (c == ' ' || c == '\n' || c == '\t')
      {
      s[i] = 0;
      }
    else
      stop = TRUE;
    i--;
    }
  }


/*==========================================================================
  vs_string_utf8_to_utf32 
*==========================================================================*/
UTF32 *vs_string_utf8_to_utf32 (const UTF8 *_in)
  {
  const UTF8* in = (UTF8 *)_in;
  int max_out = strlen ((char *)_in);
  UTF32 *out = malloc ((max_out + 1) * sizeof (UTF32));
  memset (out, 0, max_out * sizeof (UTF32));
  UTF32 *out_temp = out;

  ConvertUTF8toUTF32 ((const UTF8 **)&in, (const UTF8 *)in+strlen((char *)in),
      &out_temp, out + max_out, 0);

  int len = out_temp - out;
  out [len] = 0;
  return out;
  }


/*==========================================================================
  vs_string_utf32_to_UTF8
*==========================================================================*/
#ifdef FOO
UTF8 *vs_string_utf32_to_utf8 (const UTF32 *_in)
  {
  const UTF32 *s = _in;
  VSString *temp = vs_string_create_empty();
  int i, l = wvs_string_length_utf32 (s);
  for (i = 0; i < l; i++)
     vs_string_append_c (temp, s[i]);

  char *ret = strdup (vs_string_cstr (temp));
  vs_string_destroy (temp);
  return (UTF8 *)ret;
  }
#endif

/*==========================================================================
  vs_string_ends_with 
*==========================================================================*/
BOOL vs_string_ends_with (const VSString *self, const char *test)
  {
  BOOL ret = FALSE;
  int pos = vs_string_find (self, test);
  if (pos >= 0)
    {
    int lself = vs_string_length (self);
    int ltest = strlen (test);
    if (pos == lself - ltest)
      ret = TRUE;
    }
  return ret;
  }


/*==========================================================================
  vs_string_alpha_sort_fn
  A function to use with list_sort to sort a list of strings into 
    alphabetic (ASCII) order
*==========================================================================*/
int vs_string_alpha_sort_fn (const void *p1, const void*p2, void *user_data)
  {
  VSString *s1 = * (VSString * const *) p1;
  VSString *s2 = * (VSString * const *) p2;
  const char *ss1 = vs_string_cstr (s1);
  const char *ss2 = vs_string_cstr (s2);
  return strcmp (ss1, ss2);
  }


/*==========================================================================
  vs_string_write_to_file
*==========================================================================*/
#ifdef FOO
BOOL vs_string_write_to_file (const VSString *self, const char *file)
  {
  return file_write_from_string (file, self);
  }
#endif

/*==========================================================================
  vs_string_write_to_path
*==========================================================================*/
#ifdef FOO
BOOL vs_string_write_to_path (const VSString *self, const Path *path)
  {
  return path_write_from_string (path, self);
  }
#endif

#ifdef FOO

/*==========================================================================
  vs_string_split

  Returns a List of VSString objects. The string is split using strtok(),
  and so this method has all the limitations that strtok() has. In 
  particular, there's no way to enter an empty token -- multiple delimiters
  are collapsed into one.

  This method always returns a List, but it may be empty if the input
  string was empty.
*==========================================================================*/
List *vs_string_split (const VSString *self, const char *delim)
  {
  List * l = list_create ((ListItemFreeFn)vs_string_destroy);

  char *s = strdup (self->str);
  
  char *tok = strtok (s, delim);

  if (tok)
    {
    do
      {
      list_append (l, vs_string_create (tok));
      } while ((tok = strtok (NULL, delim)));
    }

  free (s);

  return l;
  }

/* tokenize() splits a string into tokens at white spaces.
 * double-quotes prevent spaces splitting the line, and an
 * escape \ before a space has the same effect. To get a quote
 * in the parsed output, use \". \" can also be used inside a 
 * quoted block -- "fred\"s" parses correctly with a " in the middle.
 * Parsing rules are similar to the shell, but not identical. In particular,
 * we don't support nested quotes
 * Caller must unref the list, which will always be valid
 * (but may be empty) */

// Dunno state -- usually start of line where nothing has been read
#define STATE_DUNNO 0
// White state -- eating whitespace
#define STATE_WHITE 1
// General state -- eating normal chars
#define STATE_GENERAL 2
// dquote state -- last read was a double quote 
#define STATE_DQUOTE 3
// dquote state -- last read was a double quote 
#define STATE_ESC 4
// Comment state -- ignore until EOL
#define STATE_COMMENT 5
// General char -- anything except escape, quotes, whitespace
#define CHAR_GENERAL 0
// White char -- space or tab
#define CHAR_WHITE 1
// Double quote char 
#define CHAR_DQUOTE 2
// Double quote char 
#define CHAR_ESC 3
// Hash comment
#define CHAR_HASH 4

List *vs_string_tokenize (const VSString *s)
  {
  List *argv = list_create ((ListItemFreeFn)vs_string_destroy);

  int i, l = strlen (vs_string_cstr(s));

  VSString *buff = vs_string_create_empty();

  int state = STATE_DUNNO;
  int last_state = STATE_DUNNO;

  for (i = 0; i < l; i++)
    {
    char c = s->str[i];
    int chartype = CHAR_GENERAL;
    switch (c)
      {
      case ' ': case '\t': 
        chartype = CHAR_WHITE;
        break; 

      case '"': 
        chartype = CHAR_DQUOTE;
        break; 

      case '\\': 
        chartype = CHAR_ESC;
        break; 

      case '#': 
        chartype = CHAR_HASH;
        break; 

      default:
        chartype = CHAR_GENERAL;
      }

    //printf ("Got char %c of type %d in state %d\n", c, chartype, state);

    // Note: the number of cases should be num_state * num_char_types
    switch (1000 * state + chartype)
      {
      // --- Dunno states ---
      case 1000 * STATE_DUNNO + CHAR_GENERAL:
        vs_string_append_byte (buff, c);
        state = STATE_GENERAL;
        break;

      case 1000 * STATE_DUNNO + CHAR_WHITE:
        // ws at start of line, or the first ws after
        state = STATE_WHITE;
        break;

      case 1000 * STATE_DUNNO + CHAR_DQUOTE:
        // Eat the quote and go into dqote mode
        state = STATE_DQUOTE;
        break;
 
      case 1000 * STATE_DUNNO + CHAR_ESC:
        last_state = state;
        state = STATE_ESC;
        break;

      case 1000 * STATE_DUNNO + CHAR_HASH:
        last_state = state;
        state = STATE_COMMENT;
        break;

      // --- White states ---
      case 1000 * STATE_WHITE + CHAR_GENERAL:
        // Got a char while in ws
        vs_string_append_byte (buff, c);
        state = STATE_GENERAL;
        break;

      case 1000 * STATE_WHITE + CHAR_WHITE:
        // Eat ws
        break;

      case 1000 * STATE_WHITE + CHAR_DQUOTE:
        // Eat the ws and go into dqote mode
        state = STATE_DQUOTE;
        break;
 
      case 1000 * STATE_WHITE + CHAR_ESC:
        last_state = state;
        state = STATE_ESC;
        break;

      case 1000 * STATE_WHITE + CHAR_HASH:
        last_state = state;
        state = STATE_COMMENT;
        break;

      // --- General states ---
      case 1000 * STATE_GENERAL + CHAR_GENERAL:
        // Eat normal char 
        vs_string_append_byte (buff, c);
        break;

      case 1000 * STATE_GENERAL + CHAR_WHITE:
        //Hit ws while eating characters -- this is a token
        if (strlen (buff->str))
          list_append (argv, buff);
        buff = vs_string_create_empty();
        state = STATE_WHITE;
        break;

      case 1000 * STATE_GENERAL + CHAR_DQUOTE:
        // Go into dquote mode 
        state = STATE_DQUOTE;
        break;

      case 1000 * STATE_GENERAL + CHAR_ESC:
        last_state = state;
        state = STATE_ESC;
        break;

      case 1000 * STATE_GENERAL + CHAR_HASH:
        //Hit hash while eating characters -- this is a token
        if (strlen (buff->str))
          list_append (argv, buff);
        buff = vs_string_create_empty();
        state = STATE_COMMENT;
        break;


      // --- Dquote states ---
      case 1000 * STATE_DQUOTE + CHAR_GENERAL:
        // Store the char, but remain in dquote mode
        vs_string_append_byte (buff, c);
        break;

      case 1000 * STATE_DQUOTE + CHAR_WHITE:
        // Store the ws, and remain in dquote mode
        vs_string_append_byte (buff, c);
        break;

      case 1000 * STATE_DQUOTE + CHAR_DQUOTE:
        // Leave duote mode and store token (which might be empty) 
        list_append (argv, buff);
        buff = vs_string_create_empty();
        state = STATE_DUNNO;
        break;

      case 1000 * STATE_DQUOTE + CHAR_ESC:
        last_state = state;
        state = STATE_ESC;
        //vs_string_append_byte (buff, c);
        break;

      case 1000 * STATE_DQUOTE + CHAR_HASH:
        // Keep this hash char -- it is quoted 
        vs_string_append_byte (buff, c);
        break;

      // --- Esc states ---
      case 1000 * STATE_ESC + CHAR_GENERAL:
        vs_string_append_byte (buff, c);
        state = last_state; 
        break;

      case 1000 * STATE_ESC + CHAR_WHITE:
        vs_string_append_byte (buff, c);
        state = last_state; 
        break;

      case 1000 * STATE_ESC + CHAR_DQUOTE:
        vs_string_append_byte (buff, '"');
        state = last_state; 
        break;

      case 1000 * STATE_ESC + CHAR_ESC:
        vs_string_append_byte (buff, '\\');
        state = last_state; 
        break;

      case 1000 * STATE_ESC + CHAR_HASH:
        vs_string_append_byte (buff, '#');
        state = last_state; 
        break;

      // --- Comment states ---
      case 1000 * STATE_COMMENT + CHAR_GENERAL:
        break;

      case 1000 * STATE_COMMENT + CHAR_WHITE:
        break;

      case 1000 * STATE_COMMENT + CHAR_DQUOTE:
        break;

      case 1000 * STATE_COMMENT + CHAR_ESC:
        break;

      case 1000 * STATE_COMMENT + CHAR_HASH:
        break;

      default:
        log_error 
           ("Internal error: bad state %d and char type %d in tokenize()",
          state, chartype);
      }
    }

  if (buff)
    {
    if (vs_string_length (buff) > 0)
      list_append (argv, buff);
    else
      vs_string_destroy (buff);
    }

  return argv;
  }

#endif


