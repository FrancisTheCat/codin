#include "codin.h"

[[nodiscard]]
internal Byte_Slice string_to_bytes(String str) {
  return (Byte_Slice) {
    .data = (byte *)str.data,
    .len  = str.len,
  };
}

[[nodiscard]]
internal String bytes_to_string(Byte_Slice bytes) {
  return (String) {
    .data = (char *)bytes.data,
    .len  = bytes.len,
  };
}

[[nodiscard]]
internal b8 rune_is_upper(rune r) {
  return 'A' <= r && r <= 'Z';
}

[[nodiscard]]
internal b8 rune_is_lower(rune r) {
  return 'a' <= r && r <= 'z';
}

[[nodiscard]]
internal b8 rune_is_alpha(rune r) {
  return ('a' <= r && r <= 'z') || ('A' <= r && r <= 'Z');
}

[[nodiscard]]
internal b8 rune_is_numeric(rune r) {
  return '0' <= r && r <= '9';
}

[[nodiscard]]
internal b8 rune_is_alpha_numeric(rune r) {
  return rune_is_alpha(r) || rune_is_numeric(r);
}

internal Allocator_Error string_delete(String s, Allocator allocator) {
  return mem_free((rawptr)s.data, s.len, allocator);
}

[[nodiscard]]
internal isize cstring_len(cstring s) {
  if (!s) {
    return 0;
  }
  isize len = 0;
  for (; *s; s += 1) {
    len += 1;
  }
  return len;
}

internal Allocator_Error cstring_delete(cstring s, Allocator allocator) {
  return mem_free((rawptr)s, cstring_len(s), allocator);
}

[[nodiscard]]
internal String cstring_to_string(cstring c) {
  String s;
  s.data = c;
  s.len = cstring_len(c);
  return s;
}

[[nodiscard]]
internal String cstring_to_string_clone(cstring c, Allocator allocator) {
  String s;
  s.len = cstring_len(c);
  s.data = (const char *)unwrap_err(mem_clone((rawptr)c, s.len, allocator));
  return s;
}

[[nodiscard]]
internal cstring string_to_cstring_unsafe(String s) { return s.data; }

[[nodiscard]]
internal cstring string_to_cstring_clone(String s, Allocator allocator) {
  char *data = (char *)unwrap_err(mem_alloc(s.len + 1, allocator));
  mem_tcopy(data, s.data, s.len);
  data[s.len] = 0;
  return data;
}

[[nodiscard]]
internal String string_clone(String s, Allocator allocator) {
  String ret;
  ret.data =
      (const char *)unwrap_err(mem_clone((rawptr)s.data, s.len, allocator));
  ret.len = s.len;
  return ret;
}

[[nodiscard]]
internal String strings_concatenate(String a, String b, Allocator allocator) {
  Byte_Slice tmp = bytes_concatenate(string_to_bytes(a), string_to_bytes(b), allocator);
  return bytes_to_string(tmp);
}

[[nodiscard]]
internal b8 cstring_equal(cstring a, cstring b) {
  if ((a == nil) || (b == nil)) {
    return a == b;
  }
  isize i = 0;
  for (; a[i] && b[i]; i += 1) {
    if (a[i] != b[i]) {
      return false;
    }
  }
  return a[i] == b[i];
}

[[nodiscard]]
internal b8 string_equal(String a, String b) {
  if (a.len != b.len) {
    return false;
  }

  slice_iter(a, c, i, {
    if (*c != b.data[i]) {
      return false;
    }
  });

  return true;
}

[[nodiscard]]
internal b8 string_compare_lexicographic(String a, String b) {
  slice_iter(a, c, i, {
    if (i >= b.len) {
      return false;
    }
    if (*c != b.data[i]) {
      return *c < b.data[i];
    }
  });
  return true;
}

[[nodiscard]]
internal b8 cstring_compare_lexicographic(cstring a, cstring b) {
  return string_compare_lexicographic(cstring_to_string(a),
                                      cstring_to_string(b));
}

#define string_range(str, start, end) slice_range(str, start, end)

[[nodiscard]]
internal isize string_index_byte(String str, byte b) {
  slice_iter(str, c, i, {
    if (*c == b) {
      return i;
    }
  });
  return -1;
}

[[nodiscard]]
internal Maybe_Int parse_isize(String str) {
  if (str.len == 0) {
    return (Maybe_Int){};
  }
  b8 negative = false;
  if (str.data[0] == '-') {
    negative = true;
    str.data += 1;
    str.len -= 1;
  } else if (str.data[0] == '+') {
    str.data += 1;
    str.len -= 1;
  }

  isize value = 0;
  isize f = 1 - (2 * !!negative);
  for (isize i = str.len - 1; i >= 0; i -= 1) {
    if (str.data[i] < '0' || str.data[i] > '9') {
      return (Maybe_Int){};
    }

    value += (str.data[i] - '0') * f;
    
    f *= 10;
  }

  return (Maybe_Int){.value = value, .ok = true};
}

internal isize string_copy(String dst, String src) {
  return bytes_copy(string_to_bytes(dst), string_to_bytes(src));
}

[[nodiscard]]
internal isize string_index(String str, String substr) {
  if (str.len < substr.len) {
    return -1;
  }

  for_range(i, 0, str.len - substr.len + 1) {
    if (string_equal(substr, slice_range(str, i, i + substr.len))) {
      return i;
    }
  }

  return -1;
}

[[nodiscard]]
internal String string_to_lower(String str, Allocator allocator) {
  Slice(char) out = slice_make(type_of(out), str.len, allocator);
  slice_iter(out, c, i, {
     *c = str.data[i];
     if (rune_is_upper(*c)) {
       *c += 'a' - 'A';
     }
   })
  return transmute(String, out);
}

#define string_iter(string, elem, i, BLOCK)                                    \
  {                                                                            \
    isize __string_iter_n = 0;                                                 \
    for (isize i = 0; i < (string).len; i += __string_iter_n) {                \
      rune elem = utf8_rune_at((string), i, &__string_iter_n);                 \
      if (__string_iter_n <= 0) { break; }                                     \
      { BLOCK }                                                                \
    }                                                                          \
  }
