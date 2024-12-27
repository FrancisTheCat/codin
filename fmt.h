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
//   'U': user callback
//   '%' '%'
#include "codin.h"

typedef __builtin_va_list va_list;
#define va_arg __builtin_va_arg
#define va_start __builtin_va_start
#define va_end __builtin_va_end

typedef struct {
  isize width;
  isize precision;
  b8 left_justify;
  b8 pad_zeroes;
  b8 is_pointer;
  b8 is_slice;
  char verb;
  b8 complete;
} _Formatter_Context;

typedef isize (*Format_User_Callback)(Writer const *w, rawptr);

[[nodiscard]]
internal String __format_float_to_buffer(f64 value, Byte_Slice buffer,
                                         isize precision) {
  if (value == __builtin_inf()) {
    buffer.data[0] = 'I';
    buffer.data[1] = 'n';
    buffer.data[2] = 'f';
    buffer.data[3] = 0;
    return (String){.data = (char *)buffer.data, .len = 3};
  }
  if (-value == __builtin_inf()) {
    buffer.data[0] = '-';
    buffer.data[1] = 'I';
    buffer.data[2] = 'n';
    buffer.data[3] = 'f';
    buffer.data[4] = 0;
    return (String){.data = (char *)buffer.data, .len = 4};
  }
  if (__builtin_isnan(value)) {
    buffer.data[0] = 'N';
    buffer.data[1] = 'a';
    buffer.data[2] = 'N';
    buffer.data[3] = 0;
    return (String){.data = (char *)buffer.data, .len = 3};
  }

  isize n = 0;
  if (value < 0) {
    buffer.data[n] = '-';
    n += 1;
    value *= -1;
  }

  n += format_usize_to_buffer((usize)value, slice_start(buffer, n)).len;

  if (precision != 0) {
    buffer.data[n] = '.';
    n += 1;
    isize shift = 1;
    for_range(i, 0, precision) { shift *= 10; }

    f64 comma_value = (value - (f64)(usize)value) * (f64)shift;
    if (comma_value - (f64)(usize)comma_value >= 0.5) {
      comma_value += 1;
    }

    isize n_comma =
        format_usize_to_buffer((usize)comma_value, slice_start(buffer, n)).len;
    for_range(i, n_comma, precision) { buffer.data[i + n] = '0'; }
    n += precision;
  }

  return (String){.data = (char *)buffer.data, .len = n};
}

internal isize __format_justify(Writer const *w, String s,
                                _Formatter_Context *ctx) {
  isize n = 0;
#define N 32
  local_persist char buf[N];
  for_range(i, 0, N) { buf[i] = ctx->pad_zeroes ? '0' : ' '; }
  if (ctx->left_justify) {
    n = or_return(write_string(w, s), n);
    while (n < ctx->width) {
      n +=
          or_return(write_bytes(w, (Byte_Slice){.data = (byte *)buf,
                                                .len = min(ctx->width - n, N)}),
                    n);
    }
  } else {
    while (n < ctx->width - s.len) {
      n += or_return(
          write_bytes(w, (Byte_Slice){.data = (byte *)buf,
                                      .len = min(ctx->width - s.len - n, N)}),
          n);
    }
    n = or_return(write_string(w, s), n);
  }
  return n;
#undef N
}

// returns number of consumed bytes
internal isize __format_parse_int(String format, isize *value) {
  int i = 0;
  *value = 0;
  for (; i < format.len; i += 1) {
    if (in_range(format.data[i], '0', '9' + 1)) {
      *value *= 10;
      *value += format.data[i] - '0';
    } else {
      break;
    }
  }
  return i;
}

[[nodiscard]]
internal String format_usize_to_buffer_bin(usize x, Byte_Slice buffer) {
  isize i = 0;
  b8 inner = false;
  for (usize mask = 1l << ((size_of(usize) * 8) - 1); mask != 0; mask >>= 1) {
    b8 bit = (mask & x) != 0;
    if (bit) {
      inner = true;
      buffer.data[i] = '1';
      i += 1;
    } else if (inner) {
      buffer.data[i] = '0';
      i += 1;
    }
  }

  return (String){.data = (char *)buffer.data, .len = i};
}

internal isize fmt_file_size_w(const Writer *w, isize size);
internal isize fmt_location_w(const Writer *w,
                              Source_Code_Location const *location);
internal isize fmt_time_w(const Writer *w, struct Time time);

internal isize fmt_wprintf_va(const Writer *w, String format, va_list va_args) {
  _Formatter_Context ctx;
  union {
    b32 b32;
    f64 f64;
    i32 character;
    struct Time time;
    isize isize;
    String string;
    rawptr pointer;
    cstring cstring;
    Void_Slice slice;
    Source_Code_Location location;
  } tp;
  Byte_Slice tmp_buf;
  {
    local_persist byte temp[128];
    tmp_buf = (Byte_Slice){.data = temp, .len = count_of(temp)};
  }
  String temp_str;

  Writer _null_writer = null_writer();

  if (!w) {
    w = &_null_writer;
  }

  isize n = 0;

  for (isize i = 0; i < format.len;) {
    if (format.data[i] == '%') {
      i += 1;
      if (i >= format.len) {
        return n;
      }

      for (ctx = (_Formatter_Context){.precision = 4};
           i < format.len && !ctx.complete;) {
        switch (format.data[i]) {
        case '.':
          i += 1;
          i += __format_parse_int(slice_start(format, i), &ctx.precision);
          break;

        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
          i += __format_parse_int(slice_start(format, i), &ctx.width);
          break;

        case '0':
          i += 1;
          ctx.pad_zeroes = true;
          break;

        case ' ':
          i += 1;
          ctx.pad_zeroes = false;
          break;

        case '[':
          i += 1;
          ctx.is_slice = true;
          break;

        case '-':
          i += 1;
          ctx.left_justify = true;
          break;

          // case '+':
          // case ' ':

        case 'f':
        case 'b':
        case 'B':
        case 'x':
        case 'X':
        case 'd':
        case 'c':
        case 'S':
        case 's':
        case 'M':
        case 'T':
        case 'D':
        case 'L':
        case 'U':
        case '%':
          ctx.verb = format.data[i];
          i += 1;
          ctx.complete = true;
          break;
        }
      }

      assert(ctx.complete);

      if (ctx.is_slice) {
        Void_Slice slice = va_arg(va_args, Void_Slice);

        for_range(j, 0, slice.len) {
          switch (ctx.verb) {
          case 'f':
            tp.f64 = ((f64 *)slice.data)[j];
            temp_str = __format_float_to_buffer(tp.f64, tmp_buf, ctx.precision);
            n += or_return(write_string(w, temp_str), n);
            break;

          case 'b':
            tp.isize = ((isize *)slice.data)[j];
            if (tp.isize < 0) {
              n += or_return(write_string(w, LIT("-")), n);
              tp.isize = -tp.isize;
            }
            temp_str = format_usize_to_buffer_bin(tp.isize, tmp_buf);
            n += or_return(write_string(w, temp_str), n);
            break;

          case 'B':
            tp.b32 = ((b32 *)slice.data)[j];
            n += or_return(write_cstring(w, tp.b32 ? "true" : "false"), n);
            break;

          case 'x':
            tp.isize = ((isize *)slice.data)[j];
            if (tp.isize < 0) {
              n += or_return(write_string(w, LIT("-")), n);
              tp.isize = -tp.isize;
            }
            temp_str = format_usize_to_buffer_hex(tp.isize, tmp_buf, false);
            n += or_return(write_string(w, temp_str), n);
            break;

          case 'X':
            tp.isize = ((isize *)slice.data)[j];
            if (tp.isize < 0) {
              n += or_return(write_string(w, LIT("-")), n);
              tp.isize = -tp.isize;
            }
            temp_str = format_usize_to_buffer_hex(tp.isize, tmp_buf, true);
            n += or_return(write_string(w, temp_str), n);
            break;

          case 'd':
            tp.isize = ((isize *)slice.data)[j];
            if (tp.isize < 0) {
              n += or_return(write_string(w, LIT("-")), n);
              tp.isize = -tp.isize;
            }
            temp_str = format_usize_to_buffer(tp.isize, tmp_buf);
            n += or_return(write_string(w, temp_str), n);
            break;

          // char
          case 'c':
            tp.character = ((rune *)slice.data)[j];
            write_byte(w, tp.character);
            break;

          // String
          case 'S':
            tp.string = ((String *)slice.data)[j];
            n += __format_justify(w, tp.string, &ctx);
            break;

          // cstring
          case 's':
            tp.cstring = ((cstring *)slice.data)[j];
            if (!tp.cstring) {
              tp.cstring = "<nil>";
            }
            n += __format_justify(w, cstring_to_string(tp.cstring), &ctx);
            break;

          // Memory size
          case 'M':
            tp.isize = ((isize *)slice.data)[j];
            n += fmt_file_size_w(w, tp.isize);
            break;

          // Time
          case 'T':
            tp.time = ((struct Time *)slice.data)[j];
            n += fmt_time_w(w, tp.time);
            break;

          // Duration
          case 'D':
            unimplemented();
            break;

          // Location
          case 'L':
            tp.location = ((Source_Code_Location *)slice.data)[j];
            n += fmt_location_w(w, &tp.location);
            break;

          // User callback (advanced)
          case 'U':
            Format_User_Callback proc_to_print =
                ((Format_User_Callback *)slice.data)[j];
            rawptr arg = va_arg(va_args, rawptr);
            n += proc_to_print(w, arg);
            break;
          }

          if (j != slice.len - 1) {
            n += or_return(write_string(w, LIT(", ")), n);
          }
        }
        assert(format.data[i] == ']');
        i += 1;
      } else {
        switch (ctx.verb) {
        case '%':
          n += or_return(write_byte(w, '%'), n);
          break;

        case 'f':
          tp.f64 = va_arg(va_args, f64);
          temp_str = __format_float_to_buffer(tp.f64, tmp_buf, ctx.precision);
          n += __format_justify(w, temp_str, &ctx);
          break;

        case 'b':
          tp.isize = va_arg(va_args, isize);
          if (tp.isize < 0) {
            n += or_return(write_string(w, LIT("-")), n);
            tp.isize = -tp.isize;
          }
          temp_str = format_usize_to_buffer_bin(tp.isize, tmp_buf);
          n += __format_justify(w, temp_str, &ctx);
          break;

        case 'B':
          tp.b32 = va_arg(va_args, b32);
          n += or_return(write_cstring(w, tp.b32 ? "true" : "false"), n);
          break;

        case 'x':
          tp.isize = va_arg(va_args, isize);
          if (tp.isize < 0) {
            n += or_return(write_string(w, LIT("-")), n);
            tp.isize = -tp.isize;
          }
          temp_str = format_usize_to_buffer_hex(tp.isize, tmp_buf, false);
          n += __format_justify(w, temp_str, &ctx);
          break;

        case 'X':
          tp.isize = va_arg(va_args, isize);
          if (tp.isize < 0) {
            n += or_return(write_string(w, LIT("-")), n);
            tp.isize = -tp.isize;
          }
          temp_str = format_usize_to_buffer_hex(tp.isize, tmp_buf, true);
          n += __format_justify(w, temp_str, &ctx);
          break;

        case 'd':
          tp.isize = va_arg(va_args, isize);
          if (tp.isize < 0) {
            n += or_return(write_string(w, LIT("-")), n);
            tp.isize = -tp.isize;
          }
          temp_str = format_usize_to_buffer(tp.isize, tmp_buf);
          n += __format_justify(w, temp_str, &ctx);
          break;

        // char
        case 'c':
          tp.character = va_arg(va_args, rune);
          write_byte(w, tp.character);
          break;

        // String
        case 'S':
          tp.string = va_arg(va_args, String);
          n += __format_justify(w, tp.string, &ctx);
          break;

        // cstring
        case 's':
          tp.cstring = va_arg(va_args, cstring);
          if (!tp.cstring) {
            tp.cstring = "<nil>";
          }
          n += __format_justify(w, cstring_to_string(tp.cstring), &ctx);
          break;

        // Memory size
        case 'M':
          tp.isize = va_arg(va_args, isize);
          n += fmt_file_size_w(w, tp.isize);
          break;

        // Time
        case 'T':
          tp.time = va_arg(va_args, struct Time);
          n += fmt_time_w(w, tp.time);
          break;

        // Duration
        case 'D':
          break;

        // Location
        case 'L':
          tp.location = va_arg(va_args, Source_Code_Location);
          n += fmt_location_w(w, &tp.location);
          break;

        // User callback (advanced)
        case 'U':
          Format_User_Callback proc_to_print = va_arg(va_args, Format_User_Callback);
          rawptr arg = va_arg(va_args, rawptr);
          n += proc_to_print(w, arg);
          break;
        }
      }

      slice_zero(tmp_buf);
    } else {
      isize j = i;
      for (; j < format.len; j++) {
        if (format.data[j] == '%') {
          break;
        }
      }
      n += or_return(write_string(w, slice_range(format, i, j)), n);
      i = j;
    }
  }

  return n;
}

internal isize fmt_wprintf(const Writer *w, String format, ...) {
  isize result;
  va_list va_args;
  va_start(va_args, format);

  result = fmt_wprintf_va(w, format, va_args);

  va_end(va_args);
  return result;
}

internal isize fmt_sbprintf_va(Builder *b, String format,
                               va_list va_args) {
  Writer w;
  w = writer_from_builder(b);
  return fmt_wprintf_va(&w, format, va_args);
}

internal isize fmt_sbprintf(Builder *b, String format, ...) {
  isize ret;
  va_list va_args;
  va_start(va_args, format);

  ret = fmt_sbprintf_va(b, format, va_args);

  va_end(va_args);
  return ret;
}

[[nodiscard]]
internal String fmt_aprintf_va(Allocator allocator, String format,
                               va_list va_args) {
  Builder b;
  Writer w;

  builder_init(&b, 0, 8, allocator);
  w = writer_from_builder(&b);
  fmt_wprintf_va(&w, format, va_args);

  return builder_to_string(b);
}

[[nodiscard]]
internal String fmt_aprintf(Allocator allocator, String format, ...) {
  String str;
  va_list va_args;
  va_start(va_args, format);

  str = fmt_aprintf_va(allocator, format, va_args);

  va_end(va_args);
  return str;
}

[[nodiscard]]
internal cstring fmt_caprintf_va(Allocator allocator, String format,
                                 va_list va_args) {
  Builder b;
  Writer w;

  builder_init(&b, 0, 8, allocator);
  w = writer_from_builder(&b);
  fmt_wprintf_va(&w, format, va_args);

  return builder_to_cstring(&b);
}

[[nodiscard]]
internal cstring fmt_caprintf(Allocator allocator, String format, ...) {
  cstring str;
  va_list va_args;
  va_start(va_args, format);

  str = fmt_caprintf_va(allocator, format, va_args);

  va_end(va_args);
  return str;
}

#define fmt_tprintf(format, ...)                                               \
  fmt_aprintf(context.temp_allocator, format, __VA_ARGS__)
#define fmt_tprintf_va(format, va_args)                                        \
  fmt_aprintf_va(context.temp_allocator, format, va_args)
  
#define fmt_ctprintf(format, ...)                                              \
  fmt_caprintf(context.temp_allocator, format, __VA_ARGS__)
#define fmt_ctprintf_va(format, va_args)                                       \
  fmt_caprintf_va(context.temp_allocator, format, va_args)
  

[[nodiscard]]
internal String fmt_bprintf_va(Byte_Slice buffer, String format,
                               va_list va_args) {
  Builder b;
  Writer w;
  String str;

  builder_init(&b, 0, 8, context.temp_allocator);
  w = writer_from_builder(&b);
  fmt_wprintf_va(&w, format, va_args);

  mem_copy(buffer.data, b.data, min(b.len, buffer.len));

  str.data = (char *)buffer.data;
  str.len = min(b.len, buffer.len);

  return str;
}

[[nodiscard]]
internal String fmt_bprintf(Byte_Slice buffer, String format, ...) {
  va_list va_args;
  String str;
  va_start(va_args, format);

  str = fmt_bprintf_va(buffer, format, va_args);

  va_end(va_args);
  return str;
}

internal isize fmt_fprintf_va(Fd f, String format,
                               va_list va_args) {
  Writer w = writer_from_handle(f);
  String s = fmt_tprintf_va(format, va_args);
  return or_else(write_string(&w, s), -1);
}

internal isize fmt_fprintf(Fd f, String format, ...) {
  isize ret;
  va_list va_args;
  va_start(va_args, format);

  ret = fmt_fprintf_va(f, format, va_args);

  va_end(va_args);
  return ret;
}

internal isize fmt_location_w(const Writer *w,
                              Source_Code_Location const *loc) {
  return fmt_wprintf(w, LIT("%S:%S(%d)"), loc->file, loc->proc, loc->line);
}

internal isize fmt_file_size_w(const Writer *w, isize size) {
  String file_size_prefixes[] = {LIT("b"),  LIT("Kb"), LIT("Mb"),
                                 LIT("Gb"), LIT("Tb"), LIT("Pb")};
  isize i = 0;
  isize s = 1;
  for (; (1000 * s <= size) && (i < 6);) {
    s *= 1000;
    i += 1;
  }

  f64 size_f = (f64)size;
  f64 size_s = (f64)s;

  assert(i >= 0);
  assert(i < count_of(file_size_prefixes));

  return fmt_wprintf(w, LIT("%.1f%S"), size_f / size_s, file_size_prefixes[i]);
}

internal isize fmt_time_w(const Writer *w, struct Time time) {
  Time_Value tv;
  time_get_value(time, &tv);
  return fmt_wprintf(w, LIT("%02d.%02d.%02d %02d:%02d:%02d"),
    (isize)tv.day,
    (isize)tv.month,
    (isize)tv.year,
    (isize)tv.hour + 1,
    (isize)tv.minute,
    (isize)tv.second
  );
}

#define fmt_debug(value, verb)                                                 \
  {                                                                            \
    String format = fmt_tprintf(LIT("%%S: %%%S\n"), LIT(#verb));               \
    fmt_eprintfln(format, LIT(#value), value);                                 \
  }

internal void tracking_allocator_fmt_results_w(const Writer *w,
                                               const Tracking_Allocator *t) {
  fmt_wprintf(w, LIT("Failed Allocations: %d\n"), t->failed_allocations.len);
  vector_iter(t->failed_allocations, fa, i, {
    fmt_wprintf(
      w,
      LIT("Allocation(id: %d):\nError: %S\nMode:  %S\nSize: %d\nPtr:   %x\nLoc:   %L\n\n"),
      fa->id,
      enum_to_string(Allocator_Error, fa->error),
      enum_to_string(Allocator_Mode, fa->mode),
      fa->size,
      fa->ptr,
      fa->location
    );
  });
  fmt_wprintf(w, LIT("\n"));
  fmt_wprintf(w, LIT("Leaked Allocations: %d\n"), t->allocations.len);
  hash_map_iter(t->allocations, ptr, fa, {
    fmt_wprintf(
        w,
        LIT("Allocation(id: %d):\nError: %S\nMode:  %S\nSize:  %d\nPtr: %x\nLoc:   %L\n\n"),
        fa->id,
        enum_to_string(Allocator_Error, fa->error),
        enum_to_string(Allocator_Mode, fa->mode),
        fa->size,
        ptr,
        fa->location
      );
  });
}

#define fmt_slice_w(w, _slice, _format)                                        \
  {                                                                            \
    type_of(_slice) _fmt_slice_w_slice = _slice;                               \
    fmt_wprintf(w, LIT("["));                                                  \
    String format = fmt_tprintf(LIT("%s, "), _format);                         \
    slice_iter(_fmt_slice_w_slice, elem, i, {                                  \
      if (i != _fmt_slice_w_slice.len - 1) {                                   \
        fmt_wprintf(w, format, *elem);                                         \
      } else {                                                                 \
        fmt_wprintf(w, _format, *elem);                                        \
      }                                                                        \
    });                                                                        \
    fmt_wprintf(w, LIT("]"));                                                  \
  }

//NOLINTBEGIN

#define fmt_wprint(w, str) fmt_wprintf(w, LIT("%S"), str)
#define fmt_wprintln(w, str) fmt_wprintf(w, LIT("%S\n"), str)
#define fmt_wprintfln(w, format, ...)                                          \
  (fmt_wprintf(w, format, __VA_ARGS__) + fmt_wprintf(w, LIT("\n")))

#define fmt_bprint(w, str) fmt_bprintf(w, LIT("%S"), str)
#define fmt_bprintln(w, str) fmt_bprintf(w, LIT("%S\n"), str)
#define fmt_bprintfln(w, format, ...)                                          \
  (fmt_bprintf(w, format, __VA_ARGS__) + fmt_bprintf(w, LIT("\n")))

#define fmt_sbprint(w, str) fmt_sbprintf(w, LIT("%S"), str)
#define fmt_sbprintln(w, str) fmt_sbprintf(w, LIT("%S\n"), str)
#define fmt_sbprintfln(w, format, ...)                                         \
  (fmt_sbprintf(w, format, __VA_ARGS__) + fmt_sbprintf(w, LIT("\n")))

#define fmt_caprint(w, str) fmt_caprintf(w, LIT("%S"), str)
#define fmt_caprintln(w, str) fmt_caprintf(w, LIT("%S\n"), str)
#define fmt_caprintfln(w, format, ...)                                         \
  (fmt_caprintf(w, format, __VA_ARGS__) + fmt_caprintf(w, LIT("\n")))

#define fmt_ctprint(w, str) fmt_ctprintf(w, LIT("%S"), str)
#define fmt_ctprintln(w, str) fmt_ctprintf(w, LIT("%S\n"), str)
#define fmt_ctprintfln(w, format, ...)                                         \
  (fmt_ctprintf(w, format, __VA_ARGS__) + fmt_ctprintf(w, LIT("\n")))

#define fmt_tprint(w, str) fmt_tprintf(w, LIT("%S"), str)
#define fmt_tprintln(w, str) fmt_tprintf(w, LIT("%S\n"), str)
#define fmt_tprintfln(w, format, ...)                                          \
  (fmt_tprintf(w, format, __VA_ARGS__) + fmt_tprintf(w, LIT("\n")))

#define fmt_aprint(w, str) fmt_aprintf(w, LIT("%S"), str)
#define fmt_aprintln(w, str) fmt_aprintf(w, LIT("%S\n"), str)
#define fmt_aprintfln(w, format, ...)                                          \
  (fmt_aprintf(w, format, __VA_ARGS__) + fmt_aprintf(w, LIT("\n")))

#define fmt_fprint(w, str) fmt_fprintf(w, LIT("%S"), str)
#define fmt_fprintln(w, str) fmt_fprintf(w, LIT("%S\n"), str)
#define fmt_fprintfln(w, format, ...)                                          \
  (fmt_fprintf(w, format, __VA_ARGS__) + fmt_fprintf(w, LIT("\n")))

//NOLINTEND

internal isize fmt_print(String str) {
  Builder b = builder_make(0, 8, context.temp_allocator);
  (void)fmt_sbprintf(&b, LIT("%S"), str);
  return or_else(write_bytes(&stdout, builder_to_bytes(b)), -1);
}

internal isize fmt_println(String str) {
  Builder b = builder_make(0, 8, context.temp_allocator);
  (void)fmt_sbprintf(&b, LIT("%S\n"), str);
  return or_else(write_bytes(&stdout, builder_to_bytes(b)), -1);
}

internal isize fmt_printf(String format, ...) {
  Builder b = builder_make(0, 8, context.temp_allocator);
  va_list va_args;
  va_start(va_args, format);
  (void)fmt_sbprintf_va(&b, format, va_args);
  va_end(va_args);
  return or_else(write_bytes(&stdout, builder_to_bytes(b)), -1);
}

internal isize fmt_printfln(String format, ...) {
  Builder b = builder_make(0, 8, context.temp_allocator);
  va_list va_args;
  va_start(va_args, format);
  (void)fmt_sbprintf_va(&b, format, va_args);
  va_end(va_args);
  vector_append(&b, '\n');
  return or_else(write_bytes(&stdout, builder_to_bytes(b)), -1);
}

internal isize fmt_eprint(String str) {
  Builder b = builder_make(0, 8, context.temp_allocator);
  (void)fmt_sbprintf(&b, LIT("%S"), str);
  return or_else(write_bytes(&stderr, builder_to_bytes(b)), -1);
}

internal isize fmt_eprintln(String str) {
  Builder b = builder_make(0, 8, context.temp_allocator);
  (void)fmt_sbprintf(&b, LIT("%S\n"), str);
  return or_else(write_bytes(&stderr, builder_to_bytes(b)), -1);
}

internal isize fmt_eprintf(String format, ...) {
  Builder b = builder_make(0, 8, context.temp_allocator);
  va_list va_args;
  va_start(va_args, format);
  (void)fmt_sbprintf_va(&b, format, va_args);
  va_end(va_args);
  return or_else(write_bytes(&stderr, builder_to_bytes(b)), -1);
}

internal isize fmt_eprintfln(String format, ...) {
  Builder b = builder_make(0, 8, context.temp_allocator);
  va_list va_args;
  va_start(va_args, format);
  (void)fmt_sbprintf_va(&b, format, va_args);
  va_end(va_args);
  vector_append(&b, '\n');
  return or_else(write_bytes(&stderr, builder_to_bytes(b)), -1);
}

#define fmt_panicf(...)                                                        \
  {                                                                            \
    fmt_wprintf(&stderr, LIT("Panic: "));                                      \
    fmt_wprintf(&stderr, __VA_ARGS__);                                         \
    fmt_wprintf(&stderr, LIT("\n"));                                           \
    trap();                                                                    \
  }
