#pragma once

#include "codin.h"

#define log_debug(string)       _log (LL_Debug, string, CALLER_LOCATION)
#define log_debugf(string, ...) _logf(LL_Debug, string, CALLER_LOCATION, __VA_ARGS__)

#define log_info(string)       _log (LL_Info, string, CALLER_LOCATION)
#define log_infof(string, ...) _logf(LL_Info, string, CALLER_LOCATION, __VA_ARGS__)

#define log_warn(string)       _log (LL_Warn, string, CALLER_LOCATION)
#define log_warnf(string, ...) _logf(LL_Warn, string, CALLER_LOCATION, __VA_ARGS__)

#define log_error(string)       _log (LL_Error, string, CALLER_LOCATION)
#define log_errorf(string, ...) _logf(LL_Error, string, CALLER_LOCATION, __VA_ARGS__)

#define log_fatal(string)       _log (LL_Fatal, string, CALLER_LOCATION)
#define log_fatalf(string, ...) _logf(LL_Fatal, string, CALLER_LOCATION, __VA_ARGS__)

extern void _log(Log_Level level, String string, Source_Code_Location location);

extern void _logf(Log_Level level, String string, Source_Code_Location location, ...);

extern void default_file_logger_proc(
  rawptr                      data,
  Log_Level                   level,
  String                      string,
  Source_Code_Location const *location
);

[[nodiscard]]
extern Logger create_file_logger(Fd handle);
