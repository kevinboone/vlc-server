/*======================================================================
  
  nv-vlc-client 

  nc-vlc-client/src/view_sys_info.c

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vlc-server/api-client.h>
#include <ncursesw/curses.h>
#include "message.h" 
#include "status.h" 
#include "keys.h" 
#include "view_misc.h" 
#include "app_context.h" 
#include "view_sys_info.h" 

/*======================================================================
  
  get_my_ip 

======================================================================*/
int get_my_ip (char *buffer, size_t buflen) 
  {
  int ret = -1;

  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  // Should check this return -- but will it ever fail?
  const char *test_ip = "8.8.8.8"; // Google DNS
  uint16_t dns_port = 53;
  struct sockaddr_in serv;
  memset (&serv, 0, sizeof(serv));
  serv.sin_family = AF_INET;
  serv.sin_addr.s_addr = inet_addr (test_ip);
  serv.sin_port = htons (dns_port);

  ret = connect (sock, (const struct sockaddr*) &serv, sizeof(serv));
  if (ret == 0)
    {
    struct sockaddr_in name;
    socklen_t namelen = sizeof(name);
    ret = getsockname (sock, (struct sockaddr*) &name, &namelen);

    if (ret == 0)
      inet_ntop (AF_INET, &name.sin_addr, buffer, buflen);

    close(sock);
    }

  return ret;
  }

/*======================================================================
  
  view_sys_info_server_version

======================================================================*/
static char *view_sys_info_server_version (LibVlcServerClient *lvsc)
  {
  char *message = NULL;
  VSApiError err_code;
  char *version = libvlc_server_client_get_version 
     (lvsc, &err_code, &message);
  if (version)
    return version;
  if (message)
    return message;
  return strdup ("?");
  }


/*======================================================================
  
  populate_sys_info

======================================================================*/
static VSList *populate_sys_info (LibVlcServerClient *lvsc, 
          const AppContext *context)
  {
  VSList *ret = vs_list_create (free);
  char *s;
  char ipbuff[20];
  if (get_my_ip (ipbuff, sizeof (ipbuff) + 1) != 0)
     strcpy (ipbuff, "?.?.?.?");

  if (context->local)
    asprintf (&s, "Browser URL: http://%s:%d", ipbuff, context->port);
  else
    asprintf (&s, "Browser URL: http://%s:%d", context->host, context->port);
  vs_list_append (ret, s);

  asprintf (&s, "Client version: " VERSION);
  vs_list_append (ret, s);
  char *version = view_sys_info_server_version (lvsc);
  asprintf (&s, "Server version: %s", version);
  free (version);
  vs_list_append (ret, s);

  VSApiError err_code;
  VSStorage *storage = libvlc_server_client_storage (lvsc, &err_code, NULL);
  if (storage)
    {
    int capacity_mb = vs_storage_get_capacity_mb (storage);
    int free_mb = vs_storage_get_free_mb (storage);
    int albums = vs_storage_get_albums (storage);
    int tracks = vs_storage_get_tracks (storage);

    char *s;
    if (capacity_mb > 0)
      asprintf (&s, "Storage capacity: %d Mb", capacity_mb);
    else
      asprintf (&s, "Storage capacity: ?? Mb");
    vs_list_append (ret, s);
    if (free_mb > 0)
      asprintf (&s, "Storage free: %d Mb (%d%%)", free_mb,
        capacity_mb > 0 ? free_mb * 100 / capacity_mb : 0);
    else
      asprintf (&s, "Storage free: ?? Mb");
    vs_list_append (ret, s);
    if (tracks >= 0)
      asprintf (&s, "Tracks: %d", tracks);
    else
      asprintf (&s, "Tracks: ??");
    vs_list_append (ret, s);
    if (albums >= 0)
      asprintf (&s, "Albums: %d", albums);
    else
      asprintf (&s, "Albums: ??");
    vs_list_append (ret, s);

    vs_storage_destroy (storage);
    }

  return ret;
  }


/*======================================================================
  
  select_menu 

======================================================================*/
static void select_menu (LibVlcServerClient *lvsc, const char *line, 
        const AppContext *context)
  {
  (void)lvsc; (void)line; (void)context;
  message_show ("Can't select in this page", context);
  }

/*======================================================================
  
  view_sys_info

======================================================================*/
void view_sys_info (WINDOW *main_window, LibVlcServerClient *lvsc, 
       int h, int w, int row, int col, const AppContext *context)
  {
  VSList *list = populate_sys_info (lvsc, context);

  view_list (main_window, lvsc, h, w, row, col, list, 
       select_menu, "System info", context);

  vs_list_destroy (list);
  }


