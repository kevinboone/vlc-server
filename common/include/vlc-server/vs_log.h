/*==========================================================================

  vlc-rest-server

  log.h
  
  Copyright (c)2022 Kevin Boone, GPLv3.0

*==========================================================================*/

#pragma once

#include <stdarg.h>
#include <vlc-server/vs_defs.h> 

#define VSLOG_ERROR 0
#define VSLOG_WARNING 1
#define VSLOG_INFO 2
#define VSLOG_DEBUG 3
#define VSLOG_TRACE 4 

#define IN vs_log_trace("entering %s",__PRETTY_FUNCTION__);
#define OUT vs_log_trace("leaving %s",__PRETTY_FUNCTION__);

// Note that only _one_ of the following can be TRUE; it's a bad 
//   implementation, but nasty things will happen if youset both.
extern BOOL vs_log_syslog;
extern BOOL vs_log_console;

void vs_log_error (const char *fmt,...);
void vs_log_warning (const char *fmt,...);
void vs_log_info (const char *fmt,...);
void vs_log_debug (const char *fmt,...);
void vs_log_trace (const char *fmt,...);
void vs_log_set_level (const int level);

void vs_log_set_log_syslog (const BOOL f);
void vs_log_set_log_console (const BOOL f);

void vs_log_vprintf (const int level, const char *fmt, va_list ap);





