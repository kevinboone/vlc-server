/*======================================================================
  
  vlc-rest-server

  client/src/cmd.h

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/

#pragma once

#include <vlc-server/vs_search_constraints.h>

typedef struct _CmdContext
  {
  int port;
  const char *host;
  VSSearchConstraints *mdc;
  } CmdContext;

typedef int (*CmdFn) (const char *argv0, int argc, char **new_argv, 
  const CmdContext *context);

extern int cmd_clear (const char *argv0, int argc, char **new_argv, 
  const CmdContext *context);

extern int cmd_stat (const char *argv0, int argc, char **new_argv, 
  const CmdContext *context);

extern int cmd_fullscan (const char *argv0, int argc, char **new_argv, 
  const CmdContext *context);

extern int cmd_scan (const char *argv0, int argc, char **new_argv, 
  const CmdContext *context);

extern int cmd_add_urls (const char *argv0, int argc, char **new_argv, 
  const CmdContext *context);

extern int cmd_stop (const char *argv0, int argc, char **new_argv, 
  const CmdContext *context);

extern int cmd_storage (const char *argv0, int argc, char **new_argv, 
  const CmdContext *context);

extern int cmd_pause (const char *argv0, int argc, char **new_argv, 
  const CmdContext *context);

extern int cmd_playlist (const char *argv0, int argc, char **new_argv, 
  const CmdContext *context);

extern int cmd_play (const char *argv0, int argc, char **new_argv, 
  const CmdContext *context);

extern int cmd_play_random_album (const char *argv0, int argc, char **new_argv, 
  const CmdContext *context);

extern int cmd_play_random_tracks (const char *argv0, int argc, char **new_argv, 
  const CmdContext *context);

extern int cmd_start (const char *argv0, int argc, char **new_argv, 
  const CmdContext *context);

extern int cmd_next (const char *argv0, int argc, char **new_argv, 
  const CmdContext *context);

extern int cmd_prev (const char *argv0, int argc, char **new_argv, 
  const CmdContext *context);

extern int cmd_shutdown (const char *argv0, int argc, char **new_argv, 
  const CmdContext *context);

extern int cmd_volume (const char *argv0, int argc, char **new_argv, 
  const CmdContext *context);

extern int cmd_index (const char *argv0, int argc, char **new_argv, 
  const CmdContext *context);

extern int cmd_list_files (const char *argv0, int argc, char **new_argv, 
  const CmdContext *context);

extern int cmd_list_dirs (const char *argv0, int argc, char **new_argv, 
  const CmdContext *context);

extern int cmd_list_albums (const char *argv0, int argc, char **new_argv, 
  const CmdContext *context);

extern int cmd_list_artists (const char *argv0, int argc, char **new_argv, 
  const CmdContext *context);

extern int cmd_list_composers (const char *argv0, int argc, char **new_argv, 
  const CmdContext *context);

extern int cmd_list_genres (const char *argv0, int argc, char **new_argv, 
  const CmdContext *context);

extern int cmd_list_tracks (const char *argv0, int argc, char **new_argv, 
  const CmdContext *context);

extern void cmd_handle_response (const char *argv0, 
  int err_code, char *msg);

extern int cmd_version (const char *argv0, int argc, char **new_argv, 
  const CmdContext *context);

