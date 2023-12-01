/*======================================================================
  
  nv-vlc-client 

  nc-vlc-client/src/keys.c

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <ncursesw/curses.h>

int keys_line_down = KEY_DOWN;
int keys_page_down = KEY_NPAGE;
int keys_line_up = KEY_UP;
int keys_page_up = KEY_PPAGE;
int keys_select = KEY_RIGHT; 
int keys_toggle_pause = ' ';
int keys_stop = 'X' - 64;
int keys_volume_up = '+';
int keys_volume_down = '-';
int keys_next = 'N' - 64;
int keys_prev = 'P' - 64;
int keys_play_random = 'R' - 64;
int keys_quit = KEY_LEFT; 


