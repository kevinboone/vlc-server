/*===========================================================================

  vlc-rest-server

  server/src/log.c

  Copyright (c)2022 Kevin Boone, GPL v3.0

===========================================================================*/

#define _GNU_SOURCE 1
#include <syslog.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <vlc-server/vs_log.h> 

int vs_log_level = VSLOG_DEBUG;
BOOL vs_log_syslog = FALSE;
BOOL vs_log_console = TRUE;

/*==========================================================================
logging_set_level
*==========================================================================*/
void vs_log_set_level (const int level)
  {
  vs_log_level = level;
  }

/*==========================================================================
logging_set_vs_log_syslog
*==========================================================================*/
void vs_log_set_log_syslog (const BOOL f)
  {
  vs_log_syslog = f;
  }

/*==========================================================================
logging_set_vs_log_console
*==========================================================================*/
void vs_log_set_log_console (const BOOL f)
  {
  vs_log_console = f;
  }

/*==========================================================================
vs_log_vprintf
*==========================================================================*/
void vs_log_vprintf (const int level, const char *fmt, va_list ap)
  {
  if (vs_log_console)
    {
    if (level > vs_log_level) return;
    char *str = NULL;
    vasprintf (&str, fmt, ap);
    const char *s;
    if (level == VSLOG_DEBUG) s = "DEBUG";
    else if (level == VSLOG_INFO) s = "INFO";
    else if (level == VSLOG_WARNING) s = "WARNING";
    else if (level == VSLOG_TRACE) s = "TRACE";
    else s = "ERROR";
    // I frequently put a spurious \n on the end of error messages.
    // It's easier to fix that here than in the rest of the code ;) 
    if (str[strlen(str) - 1] == '\n')
      str[strlen(str) - 1] = 0;
    printf ("%s %s\n", s, str);
    free (str);
    }
  else if (vs_log_syslog)
    {
    if (level > vs_log_level) return;
    int sys_level = LOG_ERR;
    switch (level)
      {
      case VSLOG_DEBUG: sys_level = LOG_DEBUG; break;
      case VSLOG_WARNING: sys_level = LOG_WARNING; break;
      case VSLOG_INFO: sys_level = LOG_NOTICE; break;
      case VSLOG_TRACE: sys_level = LOG_DEBUG; break;
      }
    vsyslog (sys_level, fmt, ap);
    }
  }


/*==========================================================================
vs_log_error
*==========================================================================*/
void vs_log_error (const char *fmt,...)
  {
  va_list ap;
  va_start (ap, fmt);
  vs_log_vprintf (VSLOG_ERROR,  fmt, ap);
  va_end (ap);
  }


/*==========================================================================
vs_log_warn
*==========================================================================*/
void vs_log_warning (const char *fmt,...)
  {
  va_list ap;
  va_start (ap, fmt);
  vs_log_vprintf (VSLOG_WARNING,  fmt, ap);
  va_end (ap);
  }


/*==========================================================================
vs_log_info
*==========================================================================*/
void vs_log_info (const char *fmt,...)
  {
  va_list ap;
  va_start (ap, fmt);
  vs_log_vprintf (VSLOG_INFO,  fmt, ap);
  va_end (ap);
  }


/*==========================================================================
vs_log_debug
*==========================================================================*/
void vs_log_debug (const char *fmt,...)
  {
  va_list ap;
  va_start (ap, fmt);
  vs_log_vprintf (VSLOG_DEBUG,  fmt, ap);
  va_end (ap);
  }


/*==========================================================================
vs_log_trace
*==========================================================================*/
void vs_log_trace (const char *fmt,...)
  {
  va_list ap;
  va_start (ap, fmt);
  vs_log_vprintf (VSLOG_TRACE,  fmt, ap);
  va_end (ap);
  }



