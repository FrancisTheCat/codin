#include "codin.h"

#include "strings.h"

extern Byte_Slice string_to_bytes(String str) {
  return (Byte_Slice) {
    .data = (byte *)str.data,
    .len  = str.len,
  };
}

extern String bytes_to_string(Byte_Slice bytes) {
  return (String) {
    .data = (char *)bytes.data,
    .len  = bytes.len,
  };
}

extern bool rune_is_upper(rune r) {
  return 'A' <= r && r <= 'Z';
}

extern bool rune_is_lower(rune r) {
  return 'a' <= r && r <= 'z';
}

extern bool rune_is_alpha(rune r) {
  return ('a' <= r && r <= 'z') || ('A' <= r && r <= 'Z');
}

extern bool rune_is_numeric(rune r) {
  return '0' <= r && r <= '9';
}

extern bool rune_is_whitespace(rune r) {
  switch (r) {
  case ' ':
  case '\n':
  case '\r':
  case '\t':
    return true;
  }
  return false;
}

extern bool rune_is_alpha_numeric(rune r) {
  return rune_is_alpha(r) || rune_is_numeric(r);
}

extern Allocator_Error string_delete(String s, Allocator allocator) {
  return mem_free((rawptr)s.data, s.len, allocator);
}

extern isize cstring_len(cstring s) {
  if (!s) {
    return 0;
  }
  isize len = 0;
  for (; *s; s += 1) {
    len += 1;
  }
  return len;
}

extern Allocator_Error cstring_delete(cstring s, Allocator allocator) {
  return mem_free((rawptr)s, cstring_len(s), allocator);
}

extern String cstring_to_string(cstring c) {
  if (!c) {
    return (String){0};
  }
  return (String) {
    .data = c,
    .len  = cstring_len(c),
  };
}

extern String cstring_to_string_clone(cstring c, Allocator allocator) {
  String s;
  s.len = cstring_len(c);
  s.data = (const char *)unwrap_err(mem_clone((rawptr)c, s.len, allocator));
  return s;
}

extern cstring string_to_cstring_unsafe(String s) { return s.data; }

extern cstring string_to_cstring_clone(String s, Allocator allocator) {
  char *data = (char *)unwrap_err(mem_alloc(s.len + 1, allocator));
  mem_tcopy(data, s.data, s.len);
  data[s.len] = 0;
  return data;
}

extern String string_clone(String s, Allocator allocator) {
  return (String) {
    .data = (const char *)unwrap_err(mem_clone((rawptr)s.data, s.len, allocator)),
    .len  = s.len,
  };
}

extern String strings_concatenate(String a, String b, Allocator allocator) {
  Byte_Slice tmp = bytes_concatenate(string_to_bytes(a), string_to_bytes(b), allocator);
  return bytes_to_string(tmp);
}

extern bool cstring_equal(cstring a, cstring b) {
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

extern bool string_equal(String a, String b) {
  if (a.len != b.len) {
    return false;
  }

  slice_iter_v(a, c, i, {
    if (c != b.data[i]) {
      return false;
    }
  });

  return true;
}

extern bool string_compare_lexicographic(String a, String b) {
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

extern bool cstring_compare_lexicographic(cstring a, cstring b) {
  return string_compare_lexicographic(cstring_to_string(a), cstring_to_string(b));
}

#define string_range(str, start, end) slice_range(str, start, end)

#include "immintrin.h"

extern isize string_index_byte(String str, byte b) {
  isize i;
  for (i = 0; ((uintptr)str.data + i) % 16; i += 1) {
    if (str.data[i] == b) {
      return i;
    }
  }

  __m128i comparator = _mm_set1_epi8(b);
  for (; i < (str.len & ~15ul); i += 16) {
    __m128i data = _mm_load_si128((__m128i *)&str.data[i]);
    __m128i cmp  = _mm_cmpeq_epi8(data, comparator);
    u32     mask = _mm_movemask_epi8(cmp);
    if (mask != 0) {
      return i + __builtin_ctz(mask);
    }
  }

  for (; i < str.len; i += 1) {
    if (str.data[i] == b) {
      return i;
    }
  }

  return -1;
}

extern isize string_index_rune(String str, rune r) {
  if (r < 128) {
    return string_index_byte(str, r);
  }

  char  buf[4];
  isize n = utf8_rune_encode(r, buf);

  isize i;
  for (i = 0; ((uintptr)str.data + i) % 16; i += 1) {
    if (str.data[i] == buf[0]) {
      return i;
    }
  }

  __m128i comparator = _mm_set1_epi8(buf[0]);
  for (; i < (str.len & ~15ul); i += 16) {
    __m128i data = _mm_load_si128((__m128i *)&str.data[i]);
    __m128i cmp  = _mm_cmpeq_epi8(data, comparator);
    u32     mask = _mm_movemask_epi8(cmp);
    while (mask != 0) {
      isize offset = i + __builtin_ctz(mask);
      if (offset + n > str.len) {
        return false;
      }

      bool match = true;
      for (isize j = 1; j < n; j += 1) {
        if (str.data[offset + j] != buf[j]) {
          match = false;
          break;
        }
      }

      if (match) {
        return offset;
      }

      mask -= 1 << __builtin_ctz(mask);
    }
  }

  for (; i < str.len; i += 1) {
    if (str.data[i] == buf[0]) {
      return i;
    }
  }

  return -1;
}

extern Maybe_Int parse_isize(String str) {
  if (str.len == 0) {
    return (Maybe_Int){};
  }
  bool negative = false;
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

extern isize string_copy(String dst, String src) {
  return bytes_copy(string_to_bytes(dst), string_to_bytes(src));
}

extern isize string_index(String str, String substr) {
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

extern String string_to_lower(String str, Allocator allocator) {
  Slice(char) out = slice_make(type_of(out), str.len, allocator);
  slice_iter(out, c, i, {
     *c = str.data[i];
     if (rune_is_upper(*c)) {
       *c += 'a' - 'A';
     }
   })
  return transmute(String, out);
}

extern String_Slice string_split(String str, String split, Allocator allocator) {
  Vector(String) splits;
  vector_init(&splits, 0, 8, allocator);

  loop {
    isize i = string_index(str, split);
    if (i >= 0) {
      vector_append(&splits, slice_end(str, i));
      str = slice_start(str, i + split.len);
    } else {
      break;
    }
  }
  
  return vector_to_slice(String_Slice, splits);
}

extern bool string_has_prefix(String str, String prefix) {
  if (str.len < prefix.len) {
    return false;
  }
  return string_equal(string_range(str, 0, prefix.len), prefix);
}

extern String string_trim_whitespace(String str) {
  return string_trim_whitespace_end(string_trim_whitespace_start(str));
}

extern String string_trim_whitespace_end(String str) {
  String ret = str;
  while (ret.len && rune_is_whitespace(ret.data[ret.len - 1])) {
    ret = slice_end(ret, ret.len - 1);
  }
  return ret;
}

extern String string_trim_whitespace_start(String str) {
  String ret = str;
  while (ret.len && rune_is_whitespace(ret.data[0])) {
    ret = slice_start(ret, 1);
  }
  return ret;
}

extern isize builder_write_bytes(Builder *b, Byte_Slice data) {
  vector_append_slice(b, data);
  return data.len;
}

extern isize builder_write_byte(Builder *b, byte data) {
  return builder_write_bytes(b, (Byte_Slice) { .data = &data, .len = 1 });
}

extern isize builder_write_string(Builder *b, String data) {
  return builder_write_bytes(b, string_to_bytes(data));
}

extern isize builder_write_cstring(Builder *b, cstring data) {
  return builder_write_string(b, cstring_to_string(data));
}
