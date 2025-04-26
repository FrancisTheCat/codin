#include "fmt.h"
#include "io.h"
#include "os.h"
#include "strings.h"
#include "time.h"

internal Hash_Map(rune, User_Formatter_Proc) _fmt_user_formatters = {0};

extern User_Formatter_Proc register_user_formatter(rune verb, User_Formatter_Proc proc) {
  if (!_fmt_user_formatters.cap) {
    hash_map_init(&_fmt_user_formatters, 8, nil, nil, context.allocator);
  }
  User_Formatter_Proc *old = hash_map_get(_fmt_user_formatters, verb);
  User_Formatter_Proc  ret = nil;
  if (old) {
    ret  = *old;
    *old = proc;
  } else {
    hash_map_insert(&_fmt_user_formatters, verb, proc);
  }
  return ret;
}

internal String __format_float_to_buffer(f64 value, Byte_Slice buffer, isize precision) {
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

  if (precision > 0) {
    buffer.data[n] = '.';
    n += 1;
    isize shift = 1;
    for_range(i, 0, precision) { shift *= 10; }

    f64 comma_value = (value - (f64)(usize)value) * (f64)shift;
    if (comma_value - (f64)(usize)comma_value >= 0.5) {
      comma_value += 1;
    }

    isize n_comma = format_usize_to_buffer((usize)comma_value, slice_start(buffer, n)).len;
    for_range(i, n_comma, precision) { buffer.data[i + n] = '0'; }
    n += precision;
  }

  return (String){.data = (char *)buffer.data, .len = n};
}

internal isize __format_justify(Writer const *w, String s, _Formatter_Context *ctx) {
  isize n = 0;
#define N 32
  local_persist char buf[N];
  for_range(i, 0, N) { buf[i] = ctx->pad_zeroes ? '0' : ' '; }
  if (ctx->left_justify) {
    n = or_return(write_string(w, s), n);
    while (n < ctx->width) {
      n += or_return(
        write_bytes(w, (Byte_Slice){.data = (byte *)buf, .len = min(ctx->width - n, N)}),
        n
      );
    }
  } else {
    while (n < ctx->width - s.len) {
      n += or_return(
        write_bytes(w, (Byte_Slice){.data = (byte *)buf, .len = min(ctx->width - s.len - n, N)}),
        n
      );
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

internal String format_usize_to_buffer_bin(usize x, Byte_Slice buffer) {
  isize i = 0;
  bool inner = false;
  for (usize mask = 1l << ((size_of(usize) * 8) - 1); mask != 0; mask >>= 1) {
    bool bit = (mask & x) != 0;
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
internal isize fmt_location_w(const Writer *w, Source_Code_Location const *location);
internal isize fmt_time_w(const Writer *w, Timestamp time);

extern isize _fmt_wprintf_va(const Writer *w, String format, va_list va_args, bool newline) {
  _Formatter_Context ctx;
  union {
    b32                  b32;
    f64                  f64;
    i32                  character;
    isize                isize;
    String               string;
    rawptr               pointer;
    cstring              cstring;
    Timestamp            time;
    Void_Slice           slice;
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

      for (ctx = (_Formatter_Context){.precision = 4}; i < format.len && !ctx.complete;) {
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
        case 'C':
        case '%':
          ctx.verb = format.data[i];
          i += 1;
          ctx.complete = true;
          break;

        default:
          fmt_panicf(LIT("Unknown format verb: '%c' (0x%x at offset %d in format string \"%S\")"), format.data[i], format.data[i], i, format);
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
            tp.time = ((Timestamp *)slice.data)[j];
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

          // Custom formatter
          case 'C':
            panic("Custom formatters are not supported for printing slices");
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
          tp.time = va_arg(va_args, Timestamp);
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

        // Custom Formatter
        case 'C':
          User_Formatter_Proc *proc = hash_map_get(_fmt_user_formatters, (rune)format.data[i]);
          assert_msg(proc, "Custom formatter not found!");
          i += 1;
          n += (*proc)(w, va_args);
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

  if (newline) {
    n += or_return(write_byte(w, '\n'), n);
  }

  return n;
}

extern String _fmt_aprintf_va(Allocator allocator, String format, va_list va_args, bool newline) {
  Builder b;
  Writer w;

  builder_init(&b, 0, 8, allocator);
  w = writer_from_builder(&b);
  _fmt_wprintf_va(&w, format, va_args, newline);

  return builder_to_string(b);
}

extern cstring _fmt_caprintf_va(Allocator allocator, String format, va_list va_args, bool newline) {
  Builder b;
  Writer w;

  builder_init(&b, 0, 8, allocator);
  w = writer_from_builder(&b);
  _fmt_wprintf_va(&w, format, va_args, newline);

  return builder_to_cstring(&b);
}

extern isize _fmt_sbprintf_va(Builder *b, String format, va_list va_args, bool newline) {
  Writer w;
  w = writer_from_builder(b);
  return _fmt_wprintf_va(&w, format, va_args, newline);
}

extern String _fmt_bprintf_va(Byte_Slice buffer, String format, va_list va_args, bool newline) {
  Writer w = buffer_writer(&buffer);
  String ret = { .data = (char *)buffer.data };
  ret.len =  _fmt_wprintf_va(&w, format, va_args, newline);
  return ret;
}

extern isize _fmt_fprintf_va(Fd f, String format, va_list va_args, bool newline) {
  Writer w = writer_from_handle(f);
  String s = _fmt_tprintf_va(format, va_args, newline);
  return or_else(write_string(&w, s), -1);
}

internal isize fmt_location_w(
  Writer               const *w,
  Source_Code_Location const *loc
) {
  return fmt_wprintf(w, LIT("%S:%S(%d)"), loc->file, loc->proc, loc->line);
}

internal isize fmt_file_size_w(const Writer *w, isize size) {
  String file_size_prefixes[] = {
    LIT("b"),  LIT("Kb"), LIT("Mb"),
    LIT("Gb"), LIT("Tb"), LIT("Pb"),
  };
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

internal isize fmt_time_w(const Writer *w, Timestamp time) {
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
