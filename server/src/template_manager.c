/*============================================================================

  xine-server-x 
  template_manager.c
  Copyright (c)2020 Kevin Boone
  Distributed under the terms of the GPL v3.0

============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <vlc-server/vs_defs.h> 
#include <vlc-server/vs_log.h> 
#include <vlc-server/vs_string.h> 
#include "template_manager.h" 

extern uint8_t index_html_start[] asm("_binary_docroot_index_html_start");
extern uint8_t index_html_end[] asm("_binary_docroot_index_html_end");
extern uint8_t favicon_ico_start[] asm("_binary_docroot_favicon_ico_start");
extern uint8_t favicon_ico_end[] asm("_binary_docroot_favicon_ico_end");
extern uint8_t generic_html_start[] asm("_binary_docroot_generic_html_start");
extern uint8_t generic_html_end[] asm("_binary_docroot_generic_html_end");
extern uint8_t functions_js_start[] asm("_binary_docroot_functions_js_start");
extern uint8_t functions_js_end[] asm("_binary_docroot_functions_js_end");
extern uint8_t stopbutton_png_start[] asm("_binary_docroot_stopbutton_png_start");
extern uint8_t stopbutton_png_end[] asm("_binary_docroot_stopbutton_png_end");
extern uint8_t playbutton_png_start[] asm("_binary_docroot_playbutton_png_start");
extern uint8_t playbutton_png_end[] asm("_binary_docroot_playbutton_png_end");
extern uint8_t pausebutton_png_start[] asm("_binary_docroot_pausebutton_png_start");
extern uint8_t pausebutton_png_end[] asm("_binary_docroot_pausebutton_png_end");
extern uint8_t prevbutton_png_start[] asm("_binary_docroot_prevbutton_png_start");
extern uint8_t prevbutton_png_end[] asm("_binary_docroot_prevbutton_png_end");
extern uint8_t nextbutton_png_start[] asm("_binary_docroot_nextbutton_png_start");
extern uint8_t nextbutton_png_end[] asm("_binary_docroot_nextbutton_png_end");
extern uint8_t main_css_start[] asm("_binary_docroot_main_css_start");
extern uint8_t main_css_end[] asm("_binary_docroot_main_css_end");
extern uint8_t default_cover_png_start[] asm("_binary_docroot_default_cover_png_start");
extern uint8_t default_cover_png_end[] asm("_binary_docroot_default_cover_png_end");
extern uint8_t menu_icon_png_start[] asm("_binary_docroot_menu_icon_png_start");
extern uint8_t menu_icon_png_end[] asm("_binary_docroot_menu_icon_png_end");
extern uint8_t spk_png_start[] asm("_binary_docroot_spk_png_start");
extern uint8_t spk_png_end[] asm("_binary_docroot_spk_png_end");

typedef struct 
  {
  int id;
  const char *tag;
  const uint8_t *start;
  const uint8_t *end;
  } TemplateData;

TemplateData templateData[] = 
  {
  { TEMPLATE_INDEX_HTML, "index.html", index_html_start, index_html_end },
  { TEMPLATE_GENERIC_HTML, "generic.html", generic_html_start, generic_html_end },
  { TEMPLATE_FAVICON_ICO, "favicon.ico", favicon_ico_start, favicon_ico_end },
  { TEMPLATE_FUNCTIONS_JS, "functions.js", functions_js_start, functions_js_end },
  { TEMPLATE_STOPBUTTON_PNG, "stopbutton.png", stopbutton_png_start, stopbutton_png_end },
  { TEMPLATE_PLAYBUTTON_PNG, "playbutton.png", playbutton_png_start, playbutton_png_end },
  { TEMPLATE_NEXTBUTTON_PNG, "nextbutton.png", nextbutton_png_start, nextbutton_png_end },
  { TEMPLATE_PREVBUTTON_PNG, "prevbutton.png", prevbutton_png_start, prevbutton_png_end },
  { TEMPLATE_PAUSEBUTTON_PNG, "pausebutton.png", pausebutton_png_start, pausebutton_png_end },
  { TEMPLATE_DEFAULT_COVER_PNG, "default_cover.png", default_cover_png_start, default_cover_png_end },
  { TEMPLATE_MENU_ICON_PNG, "menu_icon.png", menu_icon_png_start, menu_icon_png_end },
  { TEMPLATE_SPK_PNG, "spk.png", spk_png_start, spk_png_end },
  { TEMPLATE_MAIN_CSS, "main.css", main_css_start, main_css_end },
  { -1, NULL, NULL, NULL },
  };


/*============================================================================

  template_manager_substitute_placeholder

============================================================================*/
void template_manager_substitute_placeholder (VSString *s, 
       const char *placeholder, const char *replace)
  {
  int pl_len = strlen (placeholder) + 5;
  char *pl = malloc (pl_len);
  strcpy (pl, "%%");
  strcat (pl, placeholder);
  strcat (pl, "%%"); 
  vs_string_substitute_all_in_place (s, pl, replace); 
  free (pl);
  }

/*============================================================================

 template_manager_get_template_id_by_tag

============================================================================*/
int template_manager_get_template_id_by_tag (const char * tag)
  {
  IN
  int ret = -1;

  int i = 0;
  TemplateData *td = &templateData[i];
  while (td->id >= 0 && ret < 0) 
   {
   if (strcmp (tag, td->tag) == 0)
     ret = td->id;
   i++;
   td = &templateData[i];
   }

  OUT
  return ret;
  }

/*============================================================================

 template_manager_get_template_data_by_id

============================================================================*/
static const TemplateData *template_manager_get_template_data_by_id (int id)
  {
  IN
  const TemplateData *ret = NULL; 

  int i = 0;
  TemplateData *td = &templateData[i];
  while (td->id >= 0 && ret == NULL) 
   {
   if (id == td->id)
     ret = td;
   i++;
   td = &templateData[i];
   }

  OUT
  return ret;
  }

/*============================================================================

  template_manager_get_data_by_tag

============================================================================*/
BOOL template_manager_get_data_by_tag (const char *tag, 
        const BYTE **buff, int *len)
  {
  IN
  BOOL ret = FALSE;
  int id = template_manager_get_template_id_by_tag (tag);
  if (id >= 0)
    {
    const TemplateData *td = template_manager_get_template_data_by_id (id);
    if (td)
      {
      *buff = td->start;
      *len = (td->end - td->start);
      ret = TRUE;
      }
    }
  OUT
  return ret;
  }

/*============================================================================

  template_manager_get_pchar_by_tag

============================================================================*/
char *template_manager_get_pchar_by_tag (const char *tag)
  {
  IN
  char *ret = NULL;
  int len;
  const BYTE *buff;
  if (template_manager_get_data_by_tag (tag, &buff, &len))
    {
    ret = malloc (len + 1);
    memcpy (ret, buff, len);
    ret[len] = 0;
    }
  OUT
  return ret;
  }

/*============================================================================

  template_manager_get_string_by_tag

============================================================================*/
VSString *template_manager_get_string_by_tag (const char *tag)
  {
  IN
  int len;
  const BYTE *buff;
  VSString *ret;
  if (template_manager_get_data_by_tag (tag, &buff, &len))
    {
    char *b = malloc (len + 1);
    memcpy (b, buff, len);
    b[len] = 0;
    ret = vs_string_create_and_own (b);
    }
  OUT
  return ret;
  }





