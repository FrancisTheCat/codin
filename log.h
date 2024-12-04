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
  String str = fmt_aprintf_va(context.temp_allocator, string, va_args);
  va_end(va_args);
  _log(level, str, location);
}

internal Mutex _log_mutex;

internal void default_file_logger_proc(
  rawptr data,
  Log_Level level,
  String string,
  Source_Code_Location const *location
) {
  mutex_lock(&_log_mutex);

  Writer w = writer_from_handle(transmute(Fd, data));
  switch (level) {
  case LL_Debug:
    write_string(&w, LIT("[Debug]["));
    break;
  case LL_Info:
    write_string(&w, LIT("[Info ]["));
    break;
  case LL_Warn:
    write_string(&w, LIT("[Warn ]["));
    break;
  case LL_Error:
    write_string(&w, LIT("[Error]["));
    break;
  case LL_Fatal:
    write_string(&w, LIT("[Fatal]["));
    break;
  }
  // fmt_time_w(&w, time_now());
  // write_string(&w, LIT("]["));
  fmt_location_w(&w, location);
  write_string(&w, LIT("] "));
  write_string(&w, string);
  write_string(&w, LIT("\n"));

  mutex_unlock(&_log_mutex);
}

internal Logger create_file_logger(Fd handle) {
  return (Logger) {
    .data = transmute(rawptr, handle),
    .proc = default_file_logger_proc,
  };
}
