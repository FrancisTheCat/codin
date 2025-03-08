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
#include "time_.h"

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

extern isize fmt_wprintf_va(const Writer *w, String format, va_list va_args);

#define fmt_wprintfc(w, format, ...) fmt_wprintf(w, LIT(format), __VA_ARGS__)
#define fmt_wprintc( w, str        ) fmt_wprintf(w, LIT("%s"  ), str        )

extern isize fmt_wprintf(const Writer *w, String format, ...);

extern isize fmt_sbprintf_va(Builder *b, String format, va_list va_args);

extern isize fmt_sbprintf(Builder *b, String format, ...);

[[nodiscard]]
extern String fmt_aprintf_va(Allocator allocator, String format, va_list va_args);
[[nodiscard]]
extern String fmt_aprintf(Allocator allocator, String format, ...);
[[nodiscard]]
extern cstring fmt_caprintf_va(Allocator allocator, String format, va_list va_args);
[[nodiscard]]
extern cstring fmt_caprintf(Allocator allocator, String format, ...);

#define fmt_tprintf(format, ...)                                               \
  fmt_aprintf(context.temp_allocator, format, __VA_ARGS__)
#define fmt_tprintf_va(format, va_args)                                        \
  fmt_aprintf_va(context.temp_allocator, format, va_args)
  
#define fmt_ctprintf(format, ...)                                              \
  fmt_caprintf(context.temp_allocator, format, __VA_ARGS__)
#define fmt_ctprintf_va(format, va_args)                                       \
  fmt_caprintf_va(context.temp_allocator, format, va_args)

[[nodiscard]]
extern String fmt_bprintf_va(Byte_Slice buffer, String format, va_list va_args);
[[nodiscard]]
extern String fmt_bprintf(Byte_Slice buffer, String format, ...);
extern isize fmt_fprintf_va(Fd f, String format, va_list va_args);
extern isize fmt_fprintf(Fd f, String format, ...);

#define fmt_debug(value, verb)                                                 \
  fmt_eprintf(LIT(#value ": %" verb "\n"), value);                             \

// extern void tracking_allocator_fmt_results_w(
//   const Writer *w,
//   const Tracking_Allocator *t
// ) {
//   fmt_wprintf(w, LIT("Failed Allocations: %d\n"), t->failed_allocations.len);
//   vector_iter(t->failed_allocations, fa, i, {
//     fmt_wprintf(
//       w,
//       LIT("Allocation(id: %d):\nError: %S\nMode:  %S\nSize: %d\nPtr:   %x\nLoc:   %L\n\n"),
//       fa->id,
//       enum_to_string(Allocator_Error, fa->error),
//       enum_to_string(Allocator_Mode, fa->mode),
//       fa->size,
//       fa->ptr,
//       fa->location
//     );
//   });
//   fmt_wprintf(w, LIT("\n"));
//   fmt_wprintf(w, LIT("Leaked Allocations: %d\n"), t->allocations.len);
//   hash_map_iter(t->allocations, ptr, fa, {
//     fmt_wprintf(
//         w,
//         LIT("Allocation(id: %d):\nError: %S\nMode:  %S\nSize:  %d\nPtr:   %x\nLoc:   %L\n\n"),
//         fa->id,
//         enum_to_string(Allocator_Error, fa->error),
//         enum_to_string(Allocator_Mode, fa->mode),
//         fa->size,
//         ptr,
//         fa->location
//       );
//   });
// }

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

//NOLINTBEGIN

#define fmt_wprint(w, str) fmt_wprintf(w, LIT("%S"), str)
#define fmt_wprintln(w, str) fmt_wprintf(w, LIT("%S\n"), str)
// #define fmt_wprintfln(w, format, ...)                                          \
//   (fmt_wprintf(w, format, __VA_ARGS__) + fmt_wprintf(w, LIT("\n")))

#define fmt_bprint(w, str) fmt_bprintf(w, LIT("%S"), str)
#define fmt_bprintln(w, str) fmt_bprintf(w, LIT("%S\n"), str)
// #define fmt_bprintfln(w, format, ...)                                          \
//   (fmt_bprintf(w, format, __VA_ARGS__) + fmt_bprintf(w, LIT("\n")))

#define fmt_sbprint(w, str) fmt_sbprintf(w, LIT("%S"), str)
#define fmt_sbprintln(w, str) fmt_sbprintf(w, LIT("%S\n"), str)
// #define fmt_sbprintfln(w, format, ...)                                         \
//   (fmt_sbprintf(w, format, __VA_ARGS__) + fmt_sbprintf(w, LIT("\n")))

#define fmt_caprint(w, str) fmt_caprintf(w, LIT("%S"), str)
#define fmt_caprintln(w, str) fmt_caprintf(w, LIT("%S\n"), str)
// #define fmt_caprintfln(w, format, ...)                                         \
//   (fmt_caprintf(w, format, __VA_ARGS__) + fmt_caprintf(w, LIT("\n")))

#define fmt_ctprint(w, str) fmt_ctprintf(w, LIT("%S"), str)
#define fmt_ctprintln(w, str) fmt_ctprintf(w, LIT("%S\n"), str)
// #define fmt_ctprintfln(w, format, ...)                                         \
//   (fmt_ctprintf(w, format, __VA_ARGS__) + fmt_ctprintf(w, LIT("\n")))

#define fmt_tprint(w, str) fmt_tprintf(w, LIT("%S"), str)
#define fmt_tprintln(w, str) fmt_tprintf(w, LIT("%S\n"), str)
// #define fmt_tprintfln(w, format, ...)                                          \
//   (fmt_tprintf(w, format, __VA_ARGS__) + fmt_tprintf(w, LIT("\n")))

#define fmt_aprint(w, str) fmt_aprintf(w, LIT("%S"), str)
#define fmt_aprintln(w, str) fmt_aprintf(w, LIT("%S\n"), str)
// #define fmt_aprintfln(w, format, ...)                                          \
//   (fmt_aprintf(w, format, __VA_ARGS__) + fmt_aprintf(w, LIT("\n")))

#define fmt_fprint(w, str) fmt_fprintf(w, LIT("%S"), str)
#define fmt_fprintln(w, str) fmt_fprintf(w, LIT("%S\n"), str)
// #define fmt_fprintfln(w, format, ...)                                          \
//   (fmt_fprintf(w, format, __VA_ARGS__) + fmt_fprintf(w, LIT("\n")))

//NOLINTEND

#define fmt_printc(format) fmt_print(LIT((format)))
extern isize fmt_print(String str);
#define fmt_printlnc(format) fmt_println(LIT(format))
extern isize fmt_println(String str);
#define fmt_printfc(format, ...) fmt_printf(LIT(format), __VA_ARGS__)
extern isize fmt_printf(String format, ...);
#define fmt_printflnc(format, ...) fmt_printfln(LIT(format), __VA_ARGS__)
extern isize fmt_printfln(String format, ...);

#define fmt_eprintc(format) fmt_eprint(LIT(format))
extern isize fmt_eprint(String str);
#define fmt_eprintlnc(format) fmt_eprintln(LIT(format))
extern isize fmt_eprintln(String str);
#define fmt_eprintfc(format, ...) fmt_eprintf(LIT(format), __VA_ARGS__)
extern isize fmt_eprintf(String format, ...);
#define fmt_eprintflnc(format, ...) fmt_eprintfln(LIT(format), __VA_ARGS__)
extern isize fmt_eprintfln(String format, ...);

#define fmt_panicf(...)                                                        \
  {                                                                            \
    fmt_wprintf(&stderr, LIT("Panic: "));                                      \
    fmt_wprintf(&stderr, __VA_ARGS__);                                         \
    fmt_wprintf(&stderr, LIT("\n"));                                           \
    trap();                                                                    \
  }
