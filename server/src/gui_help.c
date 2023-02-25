/*======================================================================
  
  vlc-server

  server/src/gui_help.c

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <microhttpd.h>
#include <vlc-server/vs_log.h> 
#include <vlc-server/vs_util.h> 
#include "template_manager.h"
#include "http_util.h"
#include "gui_help.h"

/*======================================================================
  
  gui_help_get_body

======================================================================*/
VSString *gui_help_get_body (const char *path, const VSProps *arguments,
           const char *instance_name)
  {
  IN
  (void)path; (void)arguments;

  vs_log_debug ("GUI help");
  VSString *help = template_manager_get_string_by_tag ("help.html"); 

  template_manager_substitute_placeholder 
          (help, "NAME", instance_name);

  template_manager_substitute_placeholder 
          (help, "VERSION", VERSION);

  VSString *body = vs_string_create ("");
  vs_string_append (body, vs_string_cstr (help));

  vs_string_destroy (help);

  OUT
  return body;
  }


