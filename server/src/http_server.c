/*======================================================================
  
  vlc-server

  http_server.c

  Copyright (c)2023 Kevin Boone, GPL v3.0

======================================================================*/

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <microhttpd.h>
#include <vlc-server/vs_log.h> 
#include <vlc-server/vs_props.h> 
#include "api.h"
#include "http_server.h"
#include "player.h"
#include "template_manager.h"
#include "gui.h"
#include "http_util.h"
#include "cover.h"
#include "media_database.h"

#define OK_MSG "{\"status\": 0, \"message\":\"OK\"}\r\n"

/*======================================================================

  HttpServer 

======================================================================*/
struct _HttpServer
  {
  int port;
  BOOL running;
  Player *player;
  char *media_root;
  struct MHD_Daemon *mhd;
  int matches_per_page;
  const char *instance_name;
  MediaDatabase *mdb;
  };

/*======================================================================

  http_server_new  

======================================================================*/
HttpServer *http_server_new (int port, const char *media_root, 
             int matches_per_page, MediaDatabase *mdb, 
             const char *instance_name)
  {
  IN
  HttpServer *self = malloc (sizeof (HttpServer));
  self->mhd = NULL;
  self->port = port;
  self->running = FALSE;
  self->media_root = strdup (media_root);
  self->matches_per_page = matches_per_page;
  self->mdb = mdb;
  self->player = NULL;
  self->instance_name = strdup (instance_name);
  OUT
  return self;
  }

/*======================================================================

  http_server_is_running

======================================================================*/
BOOL http_server_is_running (const HttpServer *self)
  {
  IN
  return self->running;
  OUT
  }

/*======================================================================

  http_server_destroy

======================================================================*/
void http_server_destroy (HttpServer *self)
  {
  IN
  http_server_stop (self);
  if (self->media_root) free (self->media_root);
  free (self);
  OUT
  }

/*======================================================================

  http_server_make_response

======================================================================*/
static enum MHD_Result http_server_make_response 
     (struct MHD_Connection *connection, int code, char *text, 
      int len, const char *type)
  {
  IN
  struct MHD_Response *response;
  enum MHD_Result ret;
  if (len == 0) len = strlen (text);
  response = MHD_create_response_from_buffer (len,
         (void*) text, MHD_RESPMEM_MUST_FREE);

  MHD_add_response_header (response, "Content-Type",
            type);
  MHD_add_response_header (response, "Cache-Control", "no-cache");

  ret = MHD_queue_response (connection, code, response); 
  MHD_destroy_response (response);

  OUT
  return ret;
  }

/*======================================================================

  http_server_handle_api

======================================================================*/
static enum MHD_Result http_server_handle_api (const HttpServer *self, 
    struct MHD_Connection *connection, const char *url, 
    const VSProps *arguments)
  {
  if (!self->player)
    {
    return http_server_make_response 
     (connection, MHD_HTTP_BAD_REQUEST, strdup ("Not initialized\r\n"), 
        0, TYPE_TEXT);
    }

  enum MHD_Result ret;
  vs_log_debug ("API url is %s", url);
  if (strncmp (url, "add_album/", 10) == 0)
    {
    char *aret = api_add_album_js (self->player, self->mdb, url + 10);
    ret = http_server_make_response 
        (connection, MHD_HTTP_OK, aret, 0, TYPE_JSON);
    }
  else if (strncmp (url, "add/", 4) == 0)
    {
    char *aret = api_add_js (self->player, url + 4);
    ret = http_server_make_response 
        (connection, MHD_HTTP_OK, aret, 0, TYPE_JSON);
    }
  else if (strncmp (url, "play_album/", 11) == 0)
    {
    char *aret = api_play_album_js (self->player, self->mdb, url + 11);
    ret = http_server_make_response 
        (connection, MHD_HTTP_OK, aret, 0, TYPE_JSON);
    }
  else if (strncmp (url, "play/", 5) == 0)
    {
    char *aret = api_play_js (self->player, url + 5);
    ret = http_server_make_response 
        (connection, MHD_HTTP_OK, aret, 0, TYPE_JSON);
    }
  else if (strcmp (url, "volume_up") == 0)
    {
    char *aret = api_volume_up_js (self->player);
    ret = http_server_make_response 
        (connection, MHD_HTTP_OK, aret, 0, TYPE_JSON);
    }
  else if (strcmp (url, "volume_down") == 0)
    {
    char *aret = api_volume_down_js (self->player);
    ret = http_server_make_response 
        (connection, MHD_HTTP_OK, aret, 0, TYPE_JSON);
    }
  else if (strncmp (url, "volume/", 7) == 0)
    {
    int vol = atoi (url + 7);
    char *aret = api_set_volume_js (self->player, vol);
    ret = http_server_make_response 
        (connection, MHD_HTTP_OK, aret, 0, TYPE_JSON);
    }
  else if (strncmp (url, "list-files/", 11) == 0)
    {
    char *aret = api_list_files_js (self->player, self->media_root, 
          url + 11);
    ret = http_server_make_response 
        (connection, MHD_HTTP_OK, aret, 0, TYPE_JSON);
    }
  else if (strncmp (url, "list-dirs/", 10) == 0)
    {
    char *aret = api_list_dirs_js (self->player, self->media_root, 
          url + 10);
    ret = http_server_make_response 
        (connection, MHD_HTTP_OK, aret, 0, TYPE_JSON);
    }
  else if (strncmp (url, "list-albums", 11) == 0)
    {
    char *aret = api_list_albums_js (self->mdb, arguments);
    ret = http_server_make_response 
        (connection, MHD_HTTP_OK, aret, 0, TYPE_JSON);
    }
  else if (strncmp (url, "list-artists", 12) == 0)
    {
    char *aret = api_list_artists_js (self->mdb, arguments);
    ret = http_server_make_response 
        (connection, MHD_HTTP_OK, aret, 0, TYPE_JSON);
    }
  else if (strncmp (url, "list-composers", 12) == 0)
    {
    char *aret = api_list_composers_js (self->mdb, arguments);
    ret = http_server_make_response 
        (connection, MHD_HTTP_OK, aret, 0, TYPE_JSON);
    }
  else if (strncmp (url, "list-genres", 12) == 0)
    {
    char *aret = api_list_genres_js (self->mdb, arguments);
    ret = http_server_make_response 
        (connection, MHD_HTTP_OK, aret, 0, TYPE_JSON);
    }
  else if (strncmp (url, "list-tracks", 11) == 0)
    {
    char *aret = api_list_tracks_js (self->mdb, arguments);
    ret = http_server_make_response 
        (connection, MHD_HTTP_OK, aret, 0, TYPE_JSON);
    }
  else if (strncmp (url, "index/", 6) == 0)
    {
    int index = atoi (url + 6);
    char *aret = api_set_index_js (self->player, index);
    ret = http_server_make_response 
        (connection, MHD_HTTP_OK, aret, 0, TYPE_JSON);
    }
  else if (strcmp (url, "start") == 0) 
    {
    char *aret = api_start_js (self->player);
    ret = http_server_make_response 
        (connection, MHD_HTTP_OK, aret, 0, TYPE_JSON);
    }
  else if (strncmp (url, "stop", 4) == 0) 
    {
    char *aret = api_stop_js (self->player);
    ret = http_server_make_response 
        (connection, MHD_HTTP_OK, aret, 0, TYPE_JSON);
    }
  else if (strncmp (url, "stat", 4) == 0) 
    {
    char *aret = api_stat_js (self->player);
    ret = http_server_make_response 
        (connection, MHD_HTTP_OK, aret, 0, TYPE_JSON);
    }
  else if (strncmp (url, "pause", 5) == 0) 
    {
    char *aret = api_pause_js (self->player);
    ret = http_server_make_response 
        (connection, MHD_HTTP_OK, aret, 0, TYPE_JSON);
    }
  else if (strncmp (url, "toggle-pause", 12) == 0) 
    {
    char *aret = api_toggle_pause_js (self->player);
    ret = http_server_make_response 
        (connection, MHD_HTTP_OK, aret, 0, TYPE_JSON);
    }
  else if (strcmp (url, "playlist") == 0) 
    {
    char *aret = api_playlist_js (self->player);
    ret = http_server_make_response 
        (connection, MHD_HTTP_OK, aret, 0, TYPE_JSON);
    }
  else if (strcmp (url, "clear") == 0) 
    {
    char *aret = api_clear_js (self->player);
    ret = http_server_make_response 
        (connection, MHD_HTTP_OK, aret, 0, TYPE_JSON);
    }
  else if (strcmp (url, "next") == 0) 
    {
    char *aret = api_next_js (self->player);
    ret = http_server_make_response 
        (connection, MHD_HTTP_OK, aret, 0, TYPE_JSON);
    }
  else if (strcmp (url, "prev") == 0) 
    {
    char *aret = api_prev_js (self->player);
    ret = http_server_make_response 
        (connection, MHD_HTTP_OK, aret, 0, TYPE_JSON);
    }
  else if (strcmp (url, "scan") == 0) 
    {
    char *aret = api_scan_js (self->mdb, self->media_root);
    ret = http_server_make_response 
        (connection, MHD_HTTP_OK, aret, 0, TYPE_JSON);
    }
  else
    { 
    ret = http_server_make_response 
     (connection, MHD_HTTP_BAD_REQUEST, strdup ("Unknown API\r\n"), 
        0, TYPE_TEXT);
    }
  return ret;
  }

/*======================================================================

  http_server_handle_file_request

======================================================================*/
static enum MHD_Result http_server_handle_file_request 
      (struct MHD_Connection *connection, const char *url)
  {
  IN
  enum MHD_Result ret;
  vs_log_debug ("File request URL %s\n", url);
  const BYTE *buff;
  int len;
  // TODO -- work out mime type from file extension!!
  if (http_util_fetch_from_docroot (url, &buff, &len))
    {
    BYTE *_buff = malloc (len);
    memcpy (_buff, buff, len);
    const char *mime = http_util_mime_from_path (url);
    ret = http_server_make_response (connection, MHD_HTTP_OK, 
      (char *)_buff, len, mime);
    }
  else
    ret = http_server_make_response (connection, MHD_HTTP_NOT_FOUND, 
      strdup ("Not found\n"), 10, TYPE_TEXT);
  OUT
  return ret;
  }

/*======================================================================

  http_server_handle_cover_request

======================================================================*/
static enum MHD_Result http_server_handle_cover_request 
      (HttpServer *self, struct MHD_Connection *connection, const char *url)
  {
  IN
  enum MHD_Result ret;
  vs_log_debug ("GUI request URL %s\n", url);
  BYTE *buff;
  int len;
  const char *type;
  if (cover_process_request (self->media_root, url, 
      &buff, &len, &type))
    ret = http_server_make_response (connection, MHD_HTTP_OK, 
      (char *)buff, len, type); 
  else
    ret = http_server_make_response (connection, MHD_HTTP_NOT_FOUND, 
      strdup ("Not found\n"), 10, TYPE_TEXT);
  OUT
  return ret;
  }

/*======================================================================

  http_server_handle_gui_request

======================================================================*/
static enum MHD_Result http_server_handle_gui_request 
      (HttpServer *self, struct MHD_Connection *connection, const char *url,
       const VSProps *arguments)
  {
  IN
  enum MHD_Result ret;
  vs_log_debug ("GUI request URL %s\n", url);
  BYTE *buff;
  int len;
  if (gui_process_request (self->player, self->media_root, url, 
        &buff, &len, arguments, 
          self->matches_per_page, self->mdb, self->instance_name))
    ret = http_server_make_response (connection, MHD_HTTP_OK, 
      (char *)buff, len, TYPE_HTML);
  else
    ret = http_server_make_response (connection, MHD_HTTP_NOT_FOUND, 
      strdup ("Not found\n"), 10, TYPE_TEXT);
  OUT
  return ret;
  }

/*======================================================================

  http_server_header_iterator

======================================================================*/
static enum MHD_Result http_server_header_iterator (void *data, 
         enum MHD_ValueKind kind, const char *key, const char *value)
  {
  IN
  (void)data; (void)kind; (void)key; (void)value;
  //printf ("key = %s val = %s\n", key, value);
  OUT
  return MHD_YES;
  }

/*======================================================================

  http_server_argument_iterator

======================================================================*/
static enum MHD_Result http_server_argument_iterator (void *data, 
         enum MHD_ValueKind kind, const char *key, const char *value)
  {
  IN
  (void)kind; 
  VSProps *arguments = (VSProps *)data;
  vs_props_add (arguments, key, value);
  OUT
  return MHD_YES;
  }

/*======================================================================

  http_server_handle_request

======================================================================*/
static enum MHD_Result http_server_handle_request (void *_data,
      struct MHD_Connection *connection, const char *url,
      const char *method, const char *version, const char *upload_data,
      size_t *upload_data_size, void **con_cls)
  {
  IN
  (void)method; (void)version; (void)upload_data; (void)upload_data_size;
  (void)con_cls;
  HttpServer *self = (HttpServer *)_data;
  //MediaDatabase *mdb = self->mdb;
  enum MHD_Result ret;

  vs_log_debug ("Request URL %s\n", url);

  VSProps *arguments = vs_props_create();

  MHD_get_connection_values (connection, MHD_HEADER_KIND, 
     http_server_header_iterator, NULL /* TODO */);

  MHD_get_connection_values (connection, MHD_GET_ARGUMENT_KIND, 
     http_server_argument_iterator, arguments);

  if (url[0] == 0 || strcmp (url, "/") == 0)
    {
    ret = http_server_handle_file_request (connection, "index.html"); 
    }
  else if (strncmp (url, "/file/", 6) == 0)
    {
    ret = http_server_handle_file_request (connection, url + 6); 
    }
  else if (strncmp (url, "/gui/", 5) == 0)
    {
    ret = http_server_handle_gui_request (self, connection, url + 5, 
      arguments); 
    }
  else if (strncmp (url, "/cover/", 7) == 0)
    {
    ret = http_server_handle_cover_request (self, connection, url + 7); 
    }
  else if (strncmp (url, "/api/", 5) == 0)
    {
    if (strncmp (url, "/api/shutdown", 13) == 0)
      {
      vs_log_info ("Received shutdown request");
      self->running = FALSE; 
      ret = http_server_make_response (connection, MHD_HTTP_OK, 
         strdup (OK_MSG), 0, TYPE_JSON);
      }
    else
      {
      ret = http_server_handle_api (self, connection, url + 5, arguments);
      }
    }
  else
    {
    ret = http_server_make_response (connection, MHD_HTTP_BAD_REQUEST, 
       strdup ("Request URL does not start with /api\r\n"), 0, TYPE_TEXT);
    }
  
  vs_props_destroy (arguments);

  return ret;
  }

/*======================================================================

  http_server_start

======================================================================*/
BOOL http_server_start (HttpServer *self)
  {
  IN
  // TODO - check already running
  int port = self->port;
  vs_log_info ("Starting HTTP server on port %d", port);
  self->mhd = MHD_start_daemon
        (MHD_USE_THREAD_PER_CONNECTION, port, NULL, NULL,
         http_server_handle_request, self, MHD_OPTION_END);
  self->running = (self->mhd != NULL);
  OUT
  return self->running; 
  }

/*======================================================================

  http_server_stop

======================================================================*/
void http_server_stop (HttpServer *self)
  {
  IN
  if (self->mhd)
    {
    MHD_stop_daemon (self->mhd);
    self->mhd = NULL;
    }
  OUT
  }

/*======================================================================

  http_server_set_player

======================================================================*/
void http_server_set_player (HttpServer *self, Player *player)
  {
  IN
  self->player = player;
  OUT
  }


