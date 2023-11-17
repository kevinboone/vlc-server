/*======================================================================
  
  vlc-rest-server

  api/src/libvlc_server_client.c

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/
#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <vlc-server/vs_defs.h>
#include <vlc-server/vs_log.h>
#include <vlc-server/api-client.h>
#include <vlc-server/vs_string.h>
#include <vlc-server/vs_playlist.h>
#include "cJSON.h" 

#define INV_JSON_MSG "Invalid JSON from server"

struct _LibVlcServerClient
  {
  char *host;
  int port;
  CURL *curl;
  };

struct CurlMem
  {
  size_t size; // But 'int' should really be long enough :)
  char *data;
  };

/*======================================================================

  libvlc_server_client_new 

======================================================================*/
LibVlcServerClient *libvlc_server_client_new (const char *host, int port)
  {
  IN
  LibVlcServerClient *self = malloc (sizeof (LibVlcServerClient));
  self->host = strdup (host);
  self->port = port;
  self->curl = curl_easy_init();
  OUT
  return self;
  }

/*======================================================================

  libvlc_server_client_destroy

======================================================================*/
void libvlc_server_client_destroy (LibVlcServerClient  *self)
  {
  if (self->host) free (self->host);
  if (self->curl) curl_easy_cleanup (self->curl); 
  free (self);
  }

/*======================================================================

  libvlc_server_client_curl_callback

======================================================================*/
static size_t libvlc_server_client_curl_callback (void *contents, 
    size_t size, size_t nmemb, void *userp)
  {
  IN
  size_t realsize = size * nmemb;
  struct CurlMem *cm = (struct CurlMem *)userp;
  cm->data = realloc (cm->data, cm->size + realsize + 1);
  memcpy (&(cm->data[cm->size]), contents, realsize);
  cm->size += realsize;
  cm->data[cm->size] = 0;
  OUT
  return realsize; 
  }

/*======================================================================

  libvlc_server_client_request

  There are two ways this function can return:

  (1) err_code != 0; *msg will be set if it is non-null on entry. The
  return value will be NULL. Caller must free *msg.

  (2) err_code = 0; *msg will be NULL. The return value will be a new
  string (that the caller must free) containing a JSON response.

  Response (1) always indicates an error of some kind (may be internal).
  Response (2) does not necessarily indicate that no error occurred --
  the caller must parse the JSON to determine whether the requested
  succeeded in the caller's own context. Typically a sucessful request
  will return a JSON object that has zero for the "status" field.

======================================================================*/
static char *libvlc_server_client_request (const LibVlcServerClient *self, 
         const char *request, VSApiError *err_code, char **msg)
  {
  IN
  char *ret = NULL;
  char curl_error [CURL_ERROR_SIZE];

  vs_log_debug ("Making request '%s'", request);

  struct CurlMem curl_mem;
  curl_mem.size = 0L;
  curl_mem.data = malloc (1);

  char *url = NULL;
  asprintf (&url, "%s:%d/api/%s", self->host, self->port, request);
  curl_easy_setopt (self->curl, CURLOPT_URL, url);
  curl_easy_setopt (self->curl, CURLOPT_ERRORBUFFER, curl_error);
  curl_easy_setopt (self->curl, CURLOPT_WRITEFUNCTION, 
    libvlc_server_client_curl_callback);
  curl_easy_setopt (self->curl, CURLOPT_WRITEDATA, &curl_mem);
  CURLcode curl_code = curl_easy_perform (self->curl);
  if (curl_code == 0)
    {
    long http_code = 0;
    curl_easy_getinfo (self->curl, CURLINFO_RESPONSE_CODE, &http_code);
    if (http_code == 200)
      {
      // Response will be JSON
      *err_code = 0;
      ret = strdup (curl_mem.data);
      vs_log_debug ("Got OK response '%s'", ret);
      }
    else
      {
      // Response will be text 
      *err_code = VSAPI_ERR_COMMS; 
      if (msg) *msg = strdup (curl_mem.data);
      vs_log_debug ("Got error response '%s'", msg);
      }
    }
  else
    {
    switch (curl_code)
      {
      case CURLE_COULDNT_CONNECT: *err_code = VSAPI_ERR_CONNECT_SERVER; 
        break;
      default:
        *err_code = VSAPI_ERR_COMMS;
      }
    if (msg) *msg = strdup (curl_error);
    }

  if (url) free (url);
  free (curl_mem.data);
  OUT
  return ret;
  }

/*======================================================================

  libvlc_server_client_check_json

======================================================================*/
static BOOL libvlc_server_client_checkjson (cJSON *root, 
    VSApiError *err_code, char **msg)
  {
  IN
  cJSON *j  = cJSON_GetObjectItem (root, "status"); 
  *err_code = j->valueint;
  if (msg)
    {
    j = cJSON_GetObjectItem (root, "message"); 
    if (j && j->valuestring)
      {
      *msg = strdup (j->valuestring);
      char *nlpos = strchr (*msg, '\r');
      if (nlpos) *nlpos = 0;
      }
    }
  OUT
  return (*err_code == 0);
  }

/*======================================================================

  libvlc_server_client_stat

======================================================================*/
VSServerStat *libvlc_server_client_stat (LibVlcServerClient *self, 
        VSApiError *err_code, char **msg)
  {
  IN
  VSServerStat *stat = NULL;
  char *response = libvlc_server_client_request (self, "stat", err_code, msg); 
  if (*err_code == 0)
    {
    cJSON *root = cJSON_Parse (response);
    if (root)
      {
      if (libvlc_server_client_checkjson (root, err_code, msg))
        {
        VSApiTransportStatus transport_status;
  	cJSON *j = cJSON_GetObjectItem (root, "transport_status"); 
        if (j) 
          transport_status = j->valueint;
        else
          transport_status = VSAPI_TS_STOPPED; // This is really a comms error

        const char *mrl = "";
        const char *title = "";
        const char *artist = "";
        const char *album = "";
        const char *genre = "";
        const char *composer = "";
        if (transport_status != VSAPI_TS_STOPPED)
          {
  	  j = cJSON_GetObjectItem (root, "mrl"); 
          if (j && j->valuestring) 
            mrl = j->valuestring;
  	  j = cJSON_GetObjectItem (root, "title"); 
          if (j && j->valuestring) 
            title = j->valuestring;
  	  j = cJSON_GetObjectItem (root, "artist"); 
          if (j && j->valuestring) 
            artist = j->valuestring;
  	  j = cJSON_GetObjectItem (root, "album"); 
          if (j && j->valuestring) 
            album = j->valuestring;
  	  j = cJSON_GetObjectItem (root, "genre"); 
          if (j && j->valuestring) 
            genre = j->valuestring;
  	  j = cJSON_GetObjectItem (root, "composer"); 
          if (j && j->valuestring) 
            composer = j->valuestring;
          }

        int position = 0;
        int duration = 0;
  	j = cJSON_GetObjectItem (root, "position"); 
        if (j)
          position = j->valueint; 
  	j = cJSON_GetObjectItem (root, "duration"); 
        if (j)
          duration = j->valueint; 

        int index = -1;
  	j = cJSON_GetObjectItem (root, "index"); 
        if (j)
          index = j->valueint; 

        int volume = -1;
  	j = cJSON_GetObjectItem (root, "volume"); 
        if (j)
          volume = j->valueint; 

        int scanner_progress = -1;
  	j = cJSON_GetObjectItem (root, "scanner_progress"); 
        if (j)
          scanner_progress = j->valueint; 

        stat = vs_server_stat_new 
          (transport_status, mrl, position, duration, index, volume,
           title, artist, album, genre, composer, scanner_progress);
        }

      cJSON_Delete (root);
      }
    else
      {
      *err_code = VSAPI_ERR_COMMS;
      if (*msg) *msg = strdup (INV_JSON_MSG);
      }
    }
  if (response) free (response);
  return stat;
  OUT
  }

/*======================================================================

  libvlc_server_client_clear

======================================================================*/
void libvlc_server_client_clear (LibVlcServerClient *self, 
        VSApiError *err_code, char **msg)
  {
  IN
  char *response = libvlc_server_client_request (self, "clear", err_code, msg); 
  if (*err_code == 0)
    {
    cJSON *root = cJSON_Parse (response);
    if (root)
      {
      if (libvlc_server_client_checkjson (root, err_code, msg))
        {
        // Nothing to do -- playlist cleared
        }

      cJSON_Delete (root);
      }
    else
      {
      *err_code = VSAPI_ERR_COMMS;
      if (*msg) *msg = strdup (INV_JSON_MSG);
      }
    }
  if (response) free (response);
  OUT
  }

/*======================================================================

  libvlc_server_client_add_url

======================================================================*/
void libvlc_server_client_add_url (LibVlcServerClient *self, 
        VSApiError *err_code, char **msg, const char *mrl)
  {
  IN
  VSString *enc_mrl = vs_string_encode_url (mrl);
  vs_string_prepend (enc_mrl, "add/");
  char *response = libvlc_server_client_request (self, vs_string_cstr(enc_mrl), 
    err_code, msg); 
  vs_string_destroy (enc_mrl);
  if (*err_code == 0)
    {
    cJSON *root = cJSON_Parse (response);
    if (root)
      {
      if (libvlc_server_client_checkjson (root, err_code, msg))
        {
        // Nothing to do -- file added 
        }

      cJSON_Delete (root);
      }
    else
      {
      *err_code = VSAPI_ERR_COMMS;
      if (*msg) *msg = strdup (INV_JSON_MSG);
      }
    }
  if (response) free (response);
  OUT
  }

/*======================================================================

  libvlc_server_client_toggle_pause

======================================================================*/
void libvlc_server_client_toggle_pause (LibVlcServerClient *self, 
        VSApiError *err_code, char **msg)
  {
  IN
  char *response = libvlc_server_client_request (self, "toggle-pause", 
     err_code, msg); 
  if (*err_code == 0)
    {
    cJSON *root = cJSON_Parse (response);
    if (root)
      {
      if (libvlc_server_client_checkjson (root, err_code, msg))
        {
        // Nothing to do
        }

      cJSON_Delete (root);
      }
    else
      {
      *err_code = VSAPI_ERR_COMMS;
      if (*msg) *msg = strdup (INV_JSON_MSG);
      }
    }
  if (response) free (response);
  OUT
  }

/*======================================================================

  libvlc_server_client_pause

======================================================================*/
void libvlc_server_client_pause (LibVlcServerClient *self, 
        VSApiError *err_code, char **msg)
  {
  IN
  char *response = libvlc_server_client_request (self, "pause", err_code, msg); 
  if (*err_code == 0)
    {
    cJSON *root = cJSON_Parse (response);
    if (root)
      {
      if (libvlc_server_client_checkjson (root, err_code, msg))
        {
        // Nothing to do
        }

      cJSON_Delete (root);
      }
    else
      {
      *err_code = VSAPI_ERR_COMMS;
      if (*msg) *msg = strdup (INV_JSON_MSG);
      }
    }
  if (response) free (response);
  OUT
  }

/*======================================================================

  libvlc_server_client_play

======================================================================*/
void libvlc_server_client_play (LibVlcServerClient *self, 
        VSApiError *err_code, char **msg, const char *mrl)
  {
  IN
  VSString *enc_mrl = vs_string_encode_url (mrl);
  vs_string_prepend (enc_mrl, "play/");
  char *response = libvlc_server_client_request (self, vs_string_cstr(enc_mrl), 
    err_code, msg); 
  vs_string_destroy (enc_mrl);
  if (*err_code == 0)
    {
    cJSON *root = cJSON_Parse (response);
    if (root)
      {
      if (libvlc_server_client_checkjson (root, err_code, msg))
        {
        // Nothing to do -- file added 
        }

      cJSON_Delete (root);
      }
    else
      {
      *err_code = VSAPI_ERR_COMMS;
      if (*msg) *msg = strdup (INV_JSON_MSG);
      }
    }
  if (response) free (response);
  OUT
  }

/*======================================================================

  libvlc_server_client_start

======================================================================*/
void libvlc_server_client_start (LibVlcServerClient *self, 
        VSApiError *err_code, char **msg)
  {
  IN
  char *response = libvlc_server_client_request (self, "start", 
      err_code, msg); 
  if (*err_code == 0)
    {
    cJSON *root = cJSON_Parse (response);
    if (root)
      {
      if (libvlc_server_client_checkjson (root, err_code, msg))
        {
        // Nothing to do
        }

      cJSON_Delete (root);
      }
    else
      {
      *err_code = VSAPI_ERR_COMMS;
      if (*msg) *msg = strdup (INV_JSON_MSG);
      }
    }
  if (response) free (response);
  OUT
  }

/*======================================================================

  libvlc_server_client_scan

======================================================================*/
void libvlc_server_client_scan (LibVlcServerClient *self, 
        VSApiError *err_code, char **msg)
  {
  IN
  char *response = libvlc_server_client_request 
   (self, "scan", err_code, msg); 
  if (*err_code == 0)
    {
    cJSON *root = cJSON_Parse (response);
    if (root)
      {
      if (libvlc_server_client_checkjson (root, err_code, msg))
        {
        // Nothing to do
        }

      cJSON_Delete (root);
      }
    else
      {
      *err_code = VSAPI_ERR_COMMS;
      if (*msg) *msg = strdup (INV_JSON_MSG);
      }
    }
  if (response) free (response);
  OUT
  }

/*======================================================================

  libvlc_server_client_shutdown

======================================================================*/
void libvlc_server_client_shutdown (LibVlcServerClient *self, 
        VSApiError *err_code, char **msg)
  {
  IN
  char *response = libvlc_server_client_request 
   (self, "shutdown", err_code, msg); 
  if (*err_code == 0)
    {
    cJSON *root = cJSON_Parse (response);
    if (root)
      {
      if (libvlc_server_client_checkjson (root, err_code, msg))
        {
        // Nothing to do
        }

      cJSON_Delete (root);
      }
    else
      {
      *err_code = VSAPI_ERR_COMMS;
      if (*msg) *msg = strdup (INV_JSON_MSG);
      }
    }
  if (response) free (response);
  OUT
  }

/*======================================================================

  libvlc_server_client_stop

======================================================================*/
void libvlc_server_client_stop (LibVlcServerClient *self, 
        VSApiError *err_code, char **msg)
  {
  IN
  char *response = libvlc_server_client_request (self, "stop", err_code, msg); 
  if (*err_code == 0)
    {
    cJSON *root = cJSON_Parse (response);
    if (root)
      {
      if (libvlc_server_client_checkjson (root, err_code, msg))
        {
        // Nothing to do
        }

      cJSON_Delete (root);
      }
    else
      {
      *err_code = VSAPI_ERR_COMMS;
      if (*msg) *msg = strdup (INV_JSON_MSG);
      }
    }
  if (response) free (response);
  OUT
  }


/*======================================================================

  libvlc_server_client_get_playlist

======================================================================*/
VSPlaylist *libvlc_server_client_get_playlist 
        (LibVlcServerClient *self, VSApiError *err_code, char **msg)
  {
  IN
  VSPlaylist *playlist = NULL; 
  char *response = libvlc_server_client_request (self, "playlist", 
    err_code, msg); 
  if (*err_code == 0)
    {
    cJSON *root = cJSON_Parse (response);
    if (root)
      {
      if (libvlc_server_client_checkjson (root, err_code, msg))
        {
  	cJSON *j = cJSON_GetObjectItem (root, "list"); 
        if (j)
          {
          int len = cJSON_GetArraySize (j);
          playlist = vs_playlist_new (len);
          for (int i = 0; i < len; i++)
            {
            cJSON *jj = cJSON_GetArrayItem (j, i);
            //printf ("v=%s\n", jj->valuestring);
            vs_playlist_set (playlist, i, jj->valuestring);
            }
          }
        }

      cJSON_Delete (root);
      }
    else
      {
      *err_code = VSAPI_ERR_COMMS;
      if (*msg) *msg = strdup (INV_JSON_MSG);
      }
    }
  if (response) free (response);
  OUT
  return playlist;  
  }

/*======================================================================

  libvlc_server_client_next

======================================================================*/
void libvlc_server_client_next (LibVlcServerClient *self, 
        VSApiError *err_code, char **msg)
  {
  IN
  char *response = libvlc_server_client_request (self, "next", err_code, msg); 
  if (*err_code == 0)
    {
    cJSON *root = cJSON_Parse (response);
    if (root)
      {
      if (libvlc_server_client_checkjson (root, err_code, msg))
        {
        // Nothing to do
        }

      cJSON_Delete (root);
      }
    else
      {
      *err_code = VSAPI_ERR_COMMS;
      if (*msg) *msg = strdup (INV_JSON_MSG);
      }
    }
  if (response) free (response);
  OUT
  }

/*======================================================================

  libvlc_server_client_prev

======================================================================*/
void libvlc_server_client_prev (LibVlcServerClient *self, 
        VSApiError *err_code, char **msg)
  {
  IN
  char *response = libvlc_server_client_request (self, "prev", err_code, msg); 
  if (*err_code == 0)
    {
    cJSON *root = cJSON_Parse (response);
    if (root)
      {
      if (libvlc_server_client_checkjson (root, err_code, msg))
        {
        // Nothing to do
        }

      cJSON_Delete (root);
      }
    else
      {
      *err_code = VSAPI_ERR_COMMS;
      if (*msg) *msg = strdup (INV_JSON_MSG);
      }
    }
  if (response) free (response);
  OUT
  }

/*======================================================================

  libvlc_server_client_set_volume

======================================================================*/
void libvlc_server_client_set_volume (LibVlcServerClient *self, 
        VSApiError *err_code, char **msg, int vol)
  {
  IN
  char *request = NULL;
  asprintf (&request, "volume/%d", vol);
  char *response = libvlc_server_client_request (self, request, err_code, msg); 
  if (*err_code == 0)
    {
    cJSON *root = cJSON_Parse (response);
    if (root)
      {
      if (libvlc_server_client_checkjson (root, err_code, msg))
        {
        // Nothing to do
        }

      cJSON_Delete (root);
      }
    else
      {
      *err_code = VSAPI_ERR_COMMS;
      if (*msg) *msg = strdup (INV_JSON_MSG);
      }
    }
  
  if (response) free (response);
  free (request);
  OUT
  }

/*======================================================================

  libvlc_server_client_set_index

======================================================================*/
void libvlc_server_client_set_index (LibVlcServerClient *self, 
        VSApiError *err_code, char **msg, int index)
  {
  IN
  char *request = NULL;
  asprintf (&request, "index/%d", index);
  char *response = libvlc_server_client_request (self, request, err_code, msg); 
  if (*err_code == 0)
    {
    cJSON *root = cJSON_Parse (response);
    if (root)
      {
      if (libvlc_server_client_checkjson (root, err_code, msg))
        {
        // Nothing to do
        }

      cJSON_Delete (root);
      }
    else
      {
      *err_code = VSAPI_ERR_COMMS;
      if (*msg) *msg = strdup (INV_JSON_MSG);
      }
    }
  
  if (response) free (response);
  free (request);
  OUT
  }

/*======================================================================

  libvlc_server_client_list_files

======================================================================*/
VSList *libvlc_server_client_list_files 
        (const LibVlcServerClient *self, VSApiError *err_code,
           char **msg, const char *path)
  {
  IN
  VSList *list = NULL;
  char *request;
  if (path[0] == '/') path++;
  VSString *enc_path = vs_string_encode_url (path);
  asprintf (&request, "list-files/%s", vs_string_cstr (enc_path)); 
  vs_string_destroy (enc_path);
  char *response = libvlc_server_client_request (self, request, 
    err_code, msg); 
  if (*err_code == 0)
    {
    cJSON *root = cJSON_Parse (response);
    if (root)
      {
      if (libvlc_server_client_checkjson (root, err_code, msg))
        {
  	cJSON *j = cJSON_GetObjectItem (root, "list"); 
        if (j)
          {
          int len = cJSON_GetArraySize (j);
          list = vs_list_create_strings ();
          for (int i = 0; i < len; i++)
            {
            cJSON *jj = cJSON_GetArrayItem (j, i);
            vs_list_append (list, strdup (jj->valuestring));
            }
          }
        }
      cJSON_Delete (root);
      }
    else
      {
      *err_code = VSAPI_ERR_COMMS;
      if (*msg) *msg = strdup (INV_JSON_MSG);
      }
    }
  if (response) free (response);
  free (request);
  OUT
  return list;
  }

/*======================================================================

  libvlc_server_client_play_album

======================================================================*/
void libvlc_server_client_play_album
        (const LibVlcServerClient *self, VSApiError *err_code,
           char **msg, const char *album)
  {
  IN
  char *request;
  VSString *enc_album = vs_string_encode_url (album);
  asprintf (&request, "play_album/%s", vs_string_cstr (enc_album)); 
  vs_string_destroy (enc_album);
  char *response = libvlc_server_client_request (self, request, 
    err_code, msg); 
  if (*err_code == 0)
    {
    cJSON *root = cJSON_Parse (response);
    if (root)
      {
      if (libvlc_server_client_checkjson (root, err_code, msg))
        {
        // Nothing to do -- playing 
        }

      cJSON_Delete (root);
      }
    else
      {
      *err_code = VSAPI_ERR_COMMS;
      if (*msg) *msg = strdup (INV_JSON_MSG);
      }
    }
  if (response) free (response);
  free (request);
  OUT
  }

/*======================================================================

  libvlc_server_client_list_dirs

======================================================================*/
VSList *libvlc_server_client_list_dirs
        (const LibVlcServerClient *self, VSApiError *err_code,
           char **msg, const char *path)
  {
  IN
  VSList *list = NULL;
  char *request;
  if (path[0] == '/') path++;
  VSString *enc_path = vs_string_encode_url (path);
  asprintf (&request, "list-dirs/%s", vs_string_cstr (enc_path)); 
  vs_string_destroy (enc_path);
  char *response = libvlc_server_client_request (self, request, 
    err_code, msg); 
  if (*err_code == 0)
    {
    cJSON *root = cJSON_Parse (response);
    if (root)
      {
      if (libvlc_server_client_checkjson (root, err_code, msg))
        {
  	cJSON *j = cJSON_GetObjectItem (root, "list"); 
        if (j)
          {
          int len = cJSON_GetArraySize (j);
          list = vs_list_create_strings ();
          for (int i = 0; i < len; i++)
            {
            cJSON *jj = cJSON_GetArrayItem (j, i);
            vs_list_append (list, strdup (jj->valuestring));
            }
          }
        }
      cJSON_Delete (root);
      }
    else
      {
      *err_code = VSAPI_ERR_COMMS;
      if (*msg) *msg = strdup (INV_JSON_MSG);
      }
    }
  if (response) free (response);
  free (request);
  OUT
  return list;
  }

/*======================================================================

  libvlc_server_client_list_albums

======================================================================*/
VSList *libvlc_server_client_list_albums (const LibVlcServerClient *self, 
          const char *where, VSApiError *err_code, char **msg)
  {
  IN
  VSList *list = NULL;
  char *request;
  if (where)
    {
    VSString *enc_where = vs_string_encode_url (where);
    asprintf (&request, "list-albums?where=%s", vs_string_cstr (enc_where));
    vs_string_destroy (enc_where);
    }
  else
    asprintf (&request, "list-albums");
  //vs_string_destroy (enc_path);
  char *response = libvlc_server_client_request (self, request, 
    err_code, msg); 
  if (*err_code == 0)
    {
    cJSON *root = cJSON_Parse (response);
    if (root)
      {
      if (libvlc_server_client_checkjson (root, err_code, msg))
        {
  	cJSON *j = cJSON_GetObjectItem (root, "list"); 
        if (j)
          {
          int len = cJSON_GetArraySize (j);
          list = vs_list_create_strings ();
          for (int i = 0; i < len; i++)
            {
            cJSON *jj = cJSON_GetArrayItem (j, i);
            vs_list_append (list, strdup (jj->valuestring));
            }
          }
        }
      cJSON_Delete (root);
      }
    else
      {
      *err_code = VSAPI_ERR_COMMS;
      if (*msg) *msg = strdup (INV_JSON_MSG);
      }
    }
  if (response) free (response);
  free (request);
  OUT
  return list;
  }

/*======================================================================

  libvlc_server_client_list_composers

======================================================================*/
VSList *libvlc_server_client_list_composers (const LibVlcServerClient *self, 
          const char *where, VSApiError *err_code, char **msg)
  {
  IN
  VSList *list = NULL;
  char *request;
  if (where)
    {
    VSString *enc_where = vs_string_encode_url (where);
    asprintf (&request, "list-composers?where=%s", vs_string_cstr (enc_where));
    vs_string_destroy (enc_where);
    }
  else
    asprintf (&request, "list-composers");
  //vs_string_destroy (enc_path);
  char *response = libvlc_server_client_request (self, request, 
    err_code, msg); 
  if (*err_code == 0)
    {
    cJSON *root = cJSON_Parse (response);
    if (root)
      {
      if (libvlc_server_client_checkjson (root, err_code, msg))
        {
  	cJSON *j = cJSON_GetObjectItem (root, "list"); 
        if (j)
          {
          int len = cJSON_GetArraySize (j);
          list = vs_list_create_strings ();
          for (int i = 0; i < len; i++)
            {
            cJSON *jj = cJSON_GetArrayItem (j, i);
            vs_list_append (list, strdup (jj->valuestring));
            }
          }
        }
      cJSON_Delete (root);
      }
    else
      {
      *err_code = VSAPI_ERR_COMMS;
      if (*msg) *msg = strdup (INV_JSON_MSG);
      }
    }
  if (response) free (response);
  free (request);
  OUT
  return list;
  }

/*======================================================================

  libvlc_server_client_list_artists

======================================================================*/
VSList *libvlc_server_client_list_artists (const LibVlcServerClient *self, 
          const char *where, VSApiError *err_code, char **msg)
  {
  IN
  VSList *list = NULL;
  char *request;
  if (where)
    {
    VSString *enc_where = vs_string_encode_url (where);
    asprintf (&request, "list-artists?where=%s", vs_string_cstr (enc_where));
    vs_string_destroy (enc_where);
    }
  else
    asprintf (&request, "list-artists");
  //vs_string_destroy (enc_path);
  char *response = libvlc_server_client_request (self, request, 
    err_code, msg); 
  if (*err_code == 0)
    {
    cJSON *root = cJSON_Parse (response);
    if (root)
      {
      if (libvlc_server_client_checkjson (root, err_code, msg))
        {
  	cJSON *j = cJSON_GetObjectItem (root, "list"); 
        if (j)
          {
          int len = cJSON_GetArraySize (j);
          list = vs_list_create_strings ();
          for (int i = 0; i < len; i++)
            {
            cJSON *jj = cJSON_GetArrayItem (j, i);
            vs_list_append (list, strdup (jj->valuestring));
            }
          }
        }
      cJSON_Delete (root);
      }
    else
      {
      *err_code = VSAPI_ERR_COMMS;
      if (*msg) *msg = strdup (INV_JSON_MSG);
      }
    }
  if (response) free (response);
  free (request);
  OUT
  return list;
  }

/*======================================================================

  libvlc_server_client_list_genres

======================================================================*/
VSList *libvlc_server_client_list_genres (const LibVlcServerClient *self, 
          const char *where, VSApiError *err_code, char **msg)
  {
  IN
  VSList *list = NULL;
  char *request;
  if (where)
    {
    VSString *enc_where = vs_string_encode_url (where);
    asprintf (&request, "list-genres?where=%s", vs_string_cstr (enc_where));
    vs_string_destroy (enc_where);
    }
  else
    asprintf (&request, "list-genres");
  //vs_string_destroy (enc_path);
  char *response = libvlc_server_client_request (self, request, 
    err_code, msg); 
  if (*err_code == 0)
    {
    cJSON *root = cJSON_Parse (response);
    if (root)
      {
      if (libvlc_server_client_checkjson (root, err_code, msg))
        {
  	cJSON *j = cJSON_GetObjectItem (root, "list"); 
        if (j)
          {
          int len = cJSON_GetArraySize (j);
          list = vs_list_create_strings ();
          for (int i = 0; i < len; i++)
            {
            cJSON *jj = cJSON_GetArrayItem (j, i);
            vs_list_append (list, strdup (jj->valuestring));
            }
          }
        }
      cJSON_Delete (root);
      }
    else
      {
      *err_code = VSAPI_ERR_COMMS;
      if (*msg) *msg = strdup (INV_JSON_MSG);
      }
    }
  if (response) free (response);
  free (request);
  OUT
  return list;
  }

/*======================================================================

  libvlc_server_client_list_tracks

======================================================================*/
VSList *libvlc_server_client_list_tracks (const LibVlcServerClient *self, 
          const char *where, VSApiError *err_code, char **msg)
  {
  IN
  VSList *list = NULL;
  char *request;
  if (where)
    {
    VSString *enc_where = vs_string_encode_url (where);
    asprintf (&request, "list-tracks?where=%s", vs_string_cstr (enc_where));
    vs_string_destroy (enc_where);
    }
  else
    asprintf (&request, "list-tracks");
  //vs_string_destroy (enc_path);
  char *response = libvlc_server_client_request (self, request, 
    err_code, msg); 
  if (*err_code == 0)
    {
    cJSON *root = cJSON_Parse (response);
    if (root)
      {
      if (libvlc_server_client_checkjson (root, err_code, msg))
        {
  	cJSON *j = cJSON_GetObjectItem (root, "list"); 
        if (j)
          {
          int len = cJSON_GetArraySize (j);
          list = vs_list_create_strings ();
          for (int i = 0; i < len; i++)
            {
            cJSON *jj = cJSON_GetArrayItem (j, i);
            vs_list_append (list, strdup (jj->valuestring));
            }
          }
        }
      cJSON_Delete (root);
      }
    else
      {
      *err_code = VSAPI_ERR_COMMS;
      if (*msg) *msg = strdup (INV_JSON_MSG);
      }
    }
  if (response) free (response);
  free (request);
  OUT
  return list;
  }

/*======================================================================

  libvlc_server_client_volume_down

======================================================================*/
void libvlc_server_client_volume_down (LibVlcServerClient *self, 
        VSApiError *err_code, char **msg)
  {
  IN
  char *request = NULL;
  asprintf (&request, "volume_down");
  char *response = libvlc_server_client_request (self, request, err_code, msg); 
  if (*err_code == 0)
    {
    cJSON *root = cJSON_Parse (response);
    if (root)
      {
      if (libvlc_server_client_checkjson (root, err_code, msg))
        {
        // Nothing to do
        }

      cJSON_Delete (root);
      }
    else
      {
      *err_code = VSAPI_ERR_COMMS;
      if (*msg) *msg = strdup (INV_JSON_MSG);
      }
    }
  
  if (response) free (response);
  free (request);
  OUT
  }

/*======================================================================

  libvlc_server_client_volume_up

======================================================================*/
void libvlc_server_client_volume_up (LibVlcServerClient *self, 
        VSApiError *err_code, char **msg)
  {
  IN
  char *request = NULL;
  asprintf (&request, "volume_up");
  char *response = libvlc_server_client_request (self, request, err_code, msg); 
  if (*err_code == 0)
    {
    cJSON *root = cJSON_Parse (response);
    if (root)
      {
      if (libvlc_server_client_checkjson (root, err_code, msg))
        {
        // Nothing to do
        }

      cJSON_Delete (root);
      }
    else
      {
      *err_code = VSAPI_ERR_COMMS;
      if (*msg) *msg = strdup (INV_JSON_MSG);
      }
    }
  
  if (response) free (response);
  free (request);
  OUT
  }

/*======================================================================

  libvlc_server_client_get_version

======================================================================*/
char *libvlc_server_client_get_version (const LibVlcServerClient *self, 
         VSApiError *err_code, char **msg)
  {
  IN
  char *ret = NULL;
  char *request = NULL;
  asprintf (&request, "version");
  char *response = libvlc_server_client_request (self, request, err_code, msg); 
  if (*err_code == 0)
    {
    cJSON *root = cJSON_Parse (response);
    if (root)
      {
      if (libvlc_server_client_checkjson (root, err_code, msg))
        {
  	cJSON *j = cJSON_GetObjectItem (root, "version"); 
        if (j) 
          ret = strdup (j->valuestring);
        else
          {
          *err_code = VSAPI_ERR_COMMS;
          if (*msg) *msg = strdup (INV_JSON_MSG);
          ret = NULL;
          }
        }

      cJSON_Delete (root);
      }
    else
      {
      *err_code = VSAPI_ERR_COMMS;
      if (*msg) *msg = strdup (INV_JSON_MSG);
      }
    }
  
  if (response) free (response);
  free (request);
  OUT
  
  return ret;
  }

/*======================================================================

  libvlc_server_client_play_random_album

======================================================================*/
void libvlc_server_client_play_random_album
        (const LibVlcServerClient *self, VSApiError *err_code, char **msg)
  {
  IN
  char *response = libvlc_server_client_request (self, "play_random_album", err_code, msg); 
  if (*err_code == 0)
    {
    cJSON *root = cJSON_Parse (response);
    if (root)
      {
      if (libvlc_server_client_checkjson (root, err_code, msg))
        {
        // Nothing to do
        }
      cJSON_Delete (root);
      }
    else
      {
      *err_code = VSAPI_ERR_COMMS;
      if (*msg) *msg = strdup (INV_JSON_MSG);
      }
    }
  if (response) free (response);
  OUT
  }

/*======================================================================

  libvlc_server_client_play_random_tracks

======================================================================*/
void libvlc_server_client_play_random_tracks
        (const LibVlcServerClient *self, VSApiError *err_code, char **msg)
  {
  IN
  char *response = libvlc_server_client_request (self, "play_random_tracks", err_code, msg); 
  if (*err_code == 0)
    {
    cJSON *root = cJSON_Parse (response);
    if (root)
      {
      if (libvlc_server_client_checkjson (root, err_code, msg))
        {
        // Nothing to do
        }
      cJSON_Delete (root);
      }
    else
      {
      *err_code = VSAPI_ERR_COMMS;
      if (*msg) *msg = strdup (INV_JSON_MSG);
      }
    }
  if (response) free (response);
  OUT
  }

