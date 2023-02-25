/*============================================================================

  vlc-server 

  template_manager.h

  Functions for retrieving binary blobs of data from files that got
    embedded into the binary during the link process. For example: static
    HTML pages, images.

  Copyright (c)2022-3 Kevin Boone, GPL v3.0

============================================================================*/

#pragma once

#include <vlc-server/vs_defs.h> 
#include <vlc-server/vs_string.h> 

#define TEMPLATE_INDEX_HTML          0
#define TEMPLATE_FAVICON_ICO         1
#define TEMPLATE_GENERIC_HTML        2
#define TEMPLATE_FUNCTIONS_JS        3
#define TEMPLATE_STOPBUTTON_PNG      4
#define TEMPLATE_PLAYBUTTON_PNG      5
#define TEMPLATE_PREVBUTTON_PNG      6
#define TEMPLATE_NEXTBUTTON_PNG      7
#define TEMPLATE_PAUSEBUTTON_PNG     8
#define TEMPLATE_MAIN_CSS            9
#define TEMPLATE_DEFAULT_COVER_PNG   10
#define TEMPLATE_MENU_ICON_PNG       11
#define TEMPLATE_SPK_PNG             12
#define TEMPLATE_HOME_HTML           13 
#define TEMPLATE_HELP_HTML           14 

BEGIN_CDECLS

void template_manager_substitute_placeholder (VSString *s, 
       const char *placeholder, const char *replace);

/** Get the raw data for a stored file, which may be binary. No
    substitutions are performed. */
BOOL template_manager_get_data_by_tag (const char *tag, 
        const BYTE **buff, int *len);

VSString *template_manager_get_string_by_tag (const char *tag);
char *template_manager_get_pchar_by_tag (const char *tag);

END_CDECLS


