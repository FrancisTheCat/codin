#pragma once

// verbs:
//   'f': float
//   'b': binary
//   'x': hex
//   'X': HEX
//   'd': decimal
//   'c': char
//   'S': String
//   's': cstring
//   'M': file size
//   'T': time
//   'D': duration
//   'L': location
//   'Cv': where v is a character thats used to decide which formatter is used. (use `register_user_formatter`)
//   '%' '%'
#include "codin.h"

#include "io.h"
#include "os.h"
#include "strings.h"

typedef struct {
  isize width;
  isize precision;
  b8    left_justify;
  b8    pad_zeroes;
  b8    is_pointer;
  b8    is_slice;
  char  verb;
  b8    complete;
} _Formatter_Context;

typedef isize (*User_Formatter_Proc)(Writer const *w, va_list va_args);

extern User_Formatter_Proc register_user_formatter(rune verb, User_Formatter_Proc proc);

// internal isize fmt_file_size_w(const Writer *w, isize size);
// internal isize fmt_location_w(const Writer *w, Source_Code_Location const *location);
// internal isize fmt_time_w(const Writer *w, Timestamp time);

#define FMT_FAMILY(RETURN, ARG_TYPE, ARG, PREFIX)                              \
  extern RETURN _fmt_##PREFIX##printf_va(                                      \
    ARG_TYPE ARG,                                                              \
    String   format,                                                           \
    va_list  va_args,                                                          \
    b8       newline                                                           \
  );                                                                           \
                                                                               \
  internal RETURN fmt_##PREFIX##printf_va(                                     \
    ARG_TYPE ARG,                                                              \
    String   format,                                                           \
    va_list  va_args                                                           \
  ) {                                                                          \
    return _fmt_##PREFIX##printf_va(ARG, format, va_args, false);              \
  }                                                                            \
                                                                               \
  internal RETURN fmt_##PREFIX##printf(ARG_TYPE ARG, String format, ...) {     \
    RETURN ret;                                                                \
    va_list va_args;                                                           \
    va_start(va_args, format);                                                 \
                                                                               \
    ret = _fmt_##PREFIX##printf_va(ARG, format, va_args, false);               \
                                                                               \
    va_end(va_args);                                                           \
    return ret;                                                                \
  }                                                                            \
                                                                               \
  internal RETURN fmt_##PREFIX##printfc(ARG_TYPE ARG, cstring format, ...) {   \
    RETURN ret;                                                                \
    va_list va_args;                                                           \
    va_start(va_args, format);                                                 \
                                                                               \
    ret = _fmt_##PREFIX##printf_va(                                            \
      ARG,                                                                     \
      cstring_to_string(format),                                               \
      va_args,                                                                 \
      false                                                                    \
    );                                                                         \
                                                                               \
    va_end(va_args);                                                           \
    return ret;                                                                \
  }                                                                            \
                                                                               \
  internal RETURN fmt_##PREFIX##printfln(ARG_TYPE ARG, String format, ...) {   \
    RETURN ret;                                                                \
    va_list va_args;                                                           \
    va_start(va_args, format);                                                 \
                                                                               \
    ret = _fmt_##PREFIX##printf_va(ARG, format, va_args, true);                \
                                                                               \
    va_end(va_args);                                                           \
    return ret;                                                                \
  }                                                                            \
                                                                               \
  internal RETURN fmt_##PREFIX##printflnc(ARG_TYPE ARG, cstring format, ...) { \
    RETURN ret;                                                                \
    va_list va_args;                                                           \
    va_start(va_args, format);                                                 \
                                                                               \
    ret = _fmt_##PREFIX##printf_va(                                            \
      ARG,                                                                     \
      cstring_to_string(format),                                               \
      va_args,                                                                 \
      true                                                                     \
    );                                                                         \
                                                                               \
    va_end(va_args);                                                           \
    return ret;                                                                \
  }                                                                            \
                                                                               \
  internal RETURN _fmt_##PREFIX##print(ARG_TYPE ARG, String str, b8 newline) { \
    if (newline) {                                                             \
      return fmt_##PREFIX##printfln(ARG, LIT("%S"), str);                      \
    } else {                                                                   \
      return fmt_##PREFIX##printf(ARG, LIT("%S"), str);                        \
    }                                                                          \
  }                                                                            \
                                                                               \
  internal RETURN fmt_##PREFIX##print(ARG_TYPE ARG, String str) {              \
    return _fmt_##PREFIX##print(ARG, str, false);                              \
  }                                                                            \
                                                                               \
  internal RETURN fmt_##PREFIX##printc(ARG_TYPE ARG, cstring str) {            \
    return _fmt_##PREFIX##print(ARG, cstring_to_string(str), false);           \
  }                                                                            \
                                                                               \
  internal RETURN fmt_##PREFIX##println(ARG_TYPE ARG, String str) {            \
    return _fmt_##PREFIX##print(ARG, str, true);                               \
  }                                                                            \
                                                                               \
  internal RETURN fmt_##PREFIX##printlnc(ARG_TYPE ARG, cstring str) {          \
    return _fmt_##PREFIX##print(ARG, cstring_to_string(str), true);            \
  }

FMT_FAMILY(isize,   Writer const *, w, w );
FMT_FAMILY(String,  Allocator,      a, a );
FMT_FAMILY(cstring, Allocator,      a, ca);
FMT_FAMILY(isize,   Builder *,      b, sb);
FMT_FAMILY(String,  Byte_Slice,     b, b );
FMT_FAMILY(isize,   Fd,             f, f );

#define FMT_FAMILY_DEFAULT_ARG(RETURN, BASE, NAME, ARG)                        \
  internal RETURN fmt_##NAME##printf_va(                                       \
    String   format,                                                           \
    va_list  va_args                                                           \
  ) {                                                                          \
    return fmt_##BASE##printf_va(ARG, format, va_args);                        \
  }                                                                            \
                                                                               \
  internal RETURN _fmt_##NAME##printf_va(                                      \
    String   format,                                                           \
    va_list  va_args,                                                          \
    b8       newline                                                           \
  ) {                                                                          \
    return _fmt_##BASE##printf_va(ARG, format, va_args, newline);              \
  }                                                                            \
                                                                               \
  internal RETURN fmt_##NAME##printf(String format, ...) {                     \
    RETURN ret;                                                                \
    va_list va_args;                                                           \
    va_start(va_args, format);                                                 \
                                                                               \
    ret = _fmt_##BASE##printf_va(ARG, format, va_args, false);                 \
                                                                               \
    va_end(va_args);                                                           \
    return ret;                                                                \
  }                                                                            \
                                                                               \
  internal RETURN fmt_##NAME##printfc(cstring format, ...) {                   \
    RETURN ret;                                                                \
    va_list va_args;                                                           \
    va_start(va_args, format);                                                 \
                                                                               \
    ret = _fmt_##BASE##printf_va(                                              \
      ARG,                                                                     \
      cstring_to_string(format),                                               \
      va_args,                                                                 \
      false                                                                    \
    );                                                                         \
                                                                               \
    va_end(va_args);                                                           \
    return ret;                                                                \
  }                                                                            \
                                                                               \
  internal RETURN fmt_##NAME##printfln(String format, ...) {                   \
    RETURN ret;                                                                \
    va_list va_args;                                                           \
    va_start(va_args, format);                                                 \
                                                                               \
    ret = _fmt_##BASE##printf_va(ARG, format, va_args, true);                  \
                                                                               \
    va_end(va_args);                                                           \
    return ret;                                                                \
  }                                                                            \
                                                                               \
  internal RETURN fmt_##NAME##printflnc(cstring format, ...) {                 \
    RETURN ret;                                                                \
    va_list va_args;                                                           \
    va_start(va_args, format);                                                 \
                                                                               \
    ret = _fmt_##BASE##printf_va(                                              \
      ARG,                                                                     \
      cstring_to_string(format),                                               \
      va_args,                                                                 \
      true                                                                     \
    );                                                                         \
                                                                               \
    va_end(va_args);                                                           \
    return ret;                                                                \
  }                                                                            \
                                                                               \
  internal RETURN fmt_##NAME##print(String str) {                              \
    return _fmt_##BASE##print(ARG, str, false);                                \
  }                                                                            \
                                                                               \
  internal RETURN fmt_##NAME##printc(cstring str) {                            \
    return _fmt_##BASE##print(ARG, cstring_to_string(str), false);             \
  }                                                                            \
                                                                               \
  internal RETURN fmt_##NAME##println(String str) {                            \
    return _fmt_##BASE##print(ARG, str, true);                                 \
  }                                                                            \
                                                                               \
  internal RETURN fmt_##NAME##printlnc(cstring str) {                          \
    return _fmt_##BASE##print(ARG, cstring_to_string(str), true);              \
  }


FMT_FAMILY_DEFAULT_ARG(cstring, ca, ct, context.temp_allocator);
FMT_FAMILY_DEFAULT_ARG(String,   a,  t, context.temp_allocator);
FMT_FAMILY_DEFAULT_ARG(isize,    w,  e, &std_err);
FMT_FAMILY_DEFAULT_ARG(isize,    w,   , &std_out);

#define fmt_debug(value, verb)                                                 \
  fmt_eprintf(LIT(#value ": %" verb "\n"), value);                             \

// extern void tracking_allocator_fmt_results_w(
//   const Writer *w,
//   const Tracking_Allocator *t
// );

#define fmt_slice_w(_w, _slice, _format)                                       \
  {                                                                            \
    type_of(_slice) _fmt_slice_w_slice = _slice;                               \
    type_of(_w) _fmt_slice_w_w = _w;                                           \
    fmt_wprintf(_fmt_slice_w_w, LIT("["));                                     \
    String format = fmt_tprintf(LIT("%s, "), _format);                         \
    slice_iter(_fmt_slice_w_slice, elem, i, {                                  \
      if (i != _fmt_slice_w_slice.len - 1) {                                   \
        fmt_wprintf(_fmt_slice_w_w, format, *elem);                            \
      } else {                                                                 \
        fmt_wprintf(_fmt_slice_w_w, _format, *elem);                           \
      }                                                                        \
    });                                                                        \
    fmt_wprintf(_fmt_slice_w_w, LIT("]"));                                     \
  }

#define fmt_panicf(...)                                                        \
  {                                                                            \
    fmt_wprintf(&std_err, LIT("Panic: "));                                     \
    fmt_wprintf(&std_err, __VA_ARGS__);                                        \
    fmt_wprintf(&std_err, LIT("\n"));                                          \
    trap();                                                                    \
  }
