#include "log.h"

#include "sync.h"

#include "fmt.h"

extern void _log(Log_Level level, String string, Source_Code_Location location) {
  if (context.logger.proc) {
    context.logger.proc(context.logger.data, level, string, &location);
  }
}

extern void _logf(Log_Level level, String string, Source_Code_Location location, ...) {
  va_list va_args;
  va_start(va_args, location);
  String str = fmt_tprintf_va(string, va_args);
  va_end(va_args);
  _log(level, str, location);
}

internal Mutex _log_mutex;

extern void default_file_logger_proc(
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

extern Logger create_file_logger(Fd handle) {
  return (Logger) {
    .data = transmute(rawptr, handle),
    .proc = default_file_logger_proc,
  };
}
