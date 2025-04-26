#pragma once

#include "codin.h"

#include "unicode.h"

[[nodiscard]]
extern Byte_Slice string_to_bytes(String str);

[[nodiscard]]
extern String bytes_to_string(Byte_Slice bytes);

[[nodiscard]]
extern bool rune_is_upper(rune r);

[[nodiscard]]
extern bool rune_is_lower(rune r);

[[nodiscard]]
extern bool rune_is_alpha(rune r);

[[nodiscard]]
extern bool rune_is_numeric(rune r);

[[nodiscard]]
extern bool rune_is_alpha_numeric(rune r);

[[nodiscard]]
extern bool rune_is_whitespace(rune r);

extern Allocator_Error string_delete(String s, Allocator allocator);

[[nodiscard]]
extern isize cstring_len(cstring s);

extern Allocator_Error cstring_delete(cstring s, Allocator allocator);

[[nodiscard]]
extern String cstring_to_string(cstring c);

[[nodiscard]]
extern String cstring_to_string_clone(cstring c, Allocator allocator);

[[nodiscard]]
extern cstring string_to_cstring_unsafe(String s);

[[nodiscard]]
extern cstring string_to_cstring_clone(String s, Allocator allocator);

[[nodiscard]]
extern String string_clone(String s, Allocator allocator);

[[nodiscard]]
extern String strings_concatenate(String a, String b, Allocator allocator);

[[nodiscard]]
extern bool cstring_equal(cstring a, cstring b);

[[nodiscard]]
extern bool string_equal(String a, String b);

[[nodiscard]]
extern bool string_compare_lexicographic(String a, String b);

[[nodiscard]]
extern bool cstring_compare_lexicographic(cstring a, cstring b);

#define string_range(str, start, end) slice_range(str, start, end)

[[nodiscard]]
extern isize string_index_byte(String str, byte b);
[[nodiscard]]
extern isize string_index_rune(String str, rune r);

[[nodiscard]]
extern Maybe_Int parse_isize(String str);

extern isize string_copy(String dst, String src);

[[nodiscard]]
extern isize string_index(String str, String substr);

[[nodiscard]]
extern String string_to_lower(String str, Allocator allocator);

#define string_iter(string, elem, i, BLOCK...) {                               \
  isize  _string_iter_n      = 0;                                              \
  String _string_iter_string = string;                                         \
  for (isize i = 0; i < _string_iter_string.len; i += _string_iter_n) {        \
    rune elem = utf8_rune_at(_string_iter_string, i, &_string_iter_n);         \
    if (_string_iter_n <= 0) { break; }                                        \
    { BLOCK }                                                                  \
  }                                                                            \
}

[[nodiscard]]
extern bool string_has_prefix(String str, String prefix);

[[nodiscard]]
extern String string_trim_whitespace(String str);
[[nodiscard]]
extern String string_trim_whitespace_end(String str);
[[nodiscard]]
extern String string_trim_whitespace_start(String str);

[[nodiscard]]
extern String_Slice string_split(String str, String split, Allocator allocator);

#define STRING_CASE_C(s) STRING_CASE(LIT(s))
#define STRING_CASE(s) \
  } if (string_equal(_string_switch_string, s)) {

#define STRING_SWITCH(s) { String _string_switch_string = s;

#define string_lines_iterator(string, line, i, BLOCK...) {                       \
  String _string_lines_iterator_data = string;                                   \
  String line;                                                                   \
  isize i = 0;                                                                   \
  loop {                                                                         \
    if (_string_lines_iterator_data.len <= 0) {                                  \
      break;                                                                     \
    }                                                                            \
    line.data = _string_lines_iterator_data.data;                                \
    line.len  = string_index_byte(_string_lines_iterator_data, '\n');            \
    if (line.len < 0) {                                                          \
      line.len = _string_lines_iterator_data.len;                                \
    }                                                                            \
    _string_lines_iterator_data.len  -= line.len + 1;                            \
    _string_lines_iterator_data.data += line.len + 1;                            \
    i += 1;                                                                      \
    { BLOCK; }                                                                   \
  }                                                                              \
}

extern isize builder_write_bytes(Builder *b, Byte_Slice data);

extern isize builder_write_byte(Builder *b, byte data);

extern isize builder_write_string(Builder *b, String data);

extern isize builder_write_cstring(Builder *b, cstring data);
