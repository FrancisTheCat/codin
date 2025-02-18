#include "codin.h"

#include "sync.h"

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

internal void _log(Log_Level level, String string, Source_Code_Location location) {
  if (context.logger.proc) {
    context.logger.proc(context.logger.data, level, string, &location);
  }
}

internal void _logf(Log_Level level, String string, Source_Code_Location location, ...) {
  va_list va_args;
  va_start(va_args, location);
  String str = fmt_tprintf_va(string, va_args);
  va_end(va_args);
  _log(level, str, location);
}

internal Mutex _log_mutex;

internal void default_file_logger_proc(
  rawptr                      data,
  Log_Level                   level,
  String                      string,
  Source_Code_Location const *location
) {
  MUTEX_GUARD(&_log_mutex, {
    fmt_fprintf(
      (Fd)data,
      LIT("[%-5S][%T][%L] %S\n"),
      slice_start(enum_to_string(Log_Level, level), 3),
      time_now(),
      *location,
      string
    );
  });
}

[[nodiscard]]
internal Logger create_file_logger(Fd handle) {
  return (Logger) {
    .data = transmute(rawptr, handle),
    .proc = default_file_logger_proc,
  };
}
