#include "codin.h"

#include "strings.h"

#include "immintrin.h"

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
  while ((uintptr)s % 16) {
    if (!*s) {
      return len;
    }
    len += 1;
    s   += 1;
  }

  __m128i zeroes = _mm_setzero_si128();
  loop {
    __m128i data = _mm_load_si128((__m128i *)s);
    __m128i cmp  = _mm_cmpeq_epi8(data, zeroes);
    u32     mask = _mm_movemask_epi8(cmp);
    if (mask) {
      return len + __builtin_ctz(mask);
    }

    s   += 16;
    len += 16;
  }
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

  if (((uintptr)a - (uintptr)b) % 16 == 0) {
    while ((uintptr)a % 16) {
      if (*a != *b) {
        return false;
      }
      if (!*a) {
        return true;
      }

      a += 1;
      b += 1;
    }

    __m128i zeroes = _mm_setzero_si128();
    loop {
      __m128i data_a = _mm_load_si128((__m128i *)a);
      __m128i data_b = _mm_load_si128((__m128i *)b);

      __m128i cmp_a  = _mm_cmpeq_epi8(data_a, zeroes);
      u32     mask_a = _mm_movemask_epi8(cmp_a);

      if (mask_a) {
        __m128i cmp_b  = _mm_cmpeq_epi8(data_b, zeroes);
        u32     mask_b = _mm_movemask_epi8(cmp_b);
        return __builtin_ctz(mask_a) == __builtin_ctz(mask_b);
      }

      __m128i cmp  = _mm_cmpeq_epi8(data_a, data_b);
      u32     mask = _mm_movemask_epi8(cmp);
      if (mask != 0xFFFF) {
        return false;
      }

      a += 16;
      b += 16;
    }
  }
  
  return string_equal(cstring_to_string(a), cstring_to_string(b));
}

extern bool string_equal(String a, String b) {
  if (a.len != b.len) {
    return false;
  }

  isize i;
  // align for performance
  for (i = 0; (((uintptr)a.data + i) % 16 != 0) && i < a.len; i += 1) {
    if (a.data[i] != b.data[i]) {
      return false;
    }
  }

  for (; i + 16 < a.len; i += 16) {
    __m128i data_a = _mm_loadu_si128((__m128i *)&a.data[i]);
    __m128i data_b = _mm_loadu_si128((__m128i *)&b.data[i]);
    __m128i cmp    = _mm_cmpeq_epi8(data_a, data_b);
    u32     mask   = _mm_movemask_epi8(cmp);
    if (mask != 0xFFFF) {
      return false;
    }
  }

  for (; i < a.len; i += 1) {
    if (a.data[i] != b.data[i]) {
      return false;
    }
  }

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

extern isize string_index_byte(String str, byte b) {
  isize i;
  for (i = 0; (((uintptr)str.data + i) % 16 != 0) && i < str.len; i += 1) {
    if (str.data[i] == b) {
      return i;
    }
  }

  __m128i comparator = _mm_set1_epi8(b);
  for (; i + 16 < str.len; i += 16) {
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

  #define CHECK_REMAINING(offset)               \
    if ((offset) + n > str.len) {               \
      return -1;                                \
    }                                           \
                                                \
    bool match = true;                          \
    for (isize j = 1; j < n; j += 1) {          \
      if (str.data[(offset) + j] != buf[j]) {   \
        match = false;                          \
        break;                                  \
      }                                         \
    }                                           \
                                                \
    if (match) {                                \
      return (offset);                          \
    }                                           \

  isize i;
  for (i = 0; (((uintptr)str.data + i) % 16 != 0) && i < str.len; i += 1) {
    if (str.data[i] == buf[0]) {
      CHECK_REMAINING(i);
      i += n - 1;
    }
  }

  __m128i comparator = _mm_set1_epi8(buf[0]);
  for (; i + 16 < str.len; i += 16) {
    __m128i data = _mm_load_si128((__m128i *)&str.data[i]);
    __m128i cmp  = _mm_cmpeq_epi8(data, comparator);
    u32     mask = _mm_movemask_epi8(cmp);
    while (mask != 0) {
      isize offset = i + __builtin_ctz(mask);
      CHECK_REMAINING(offset);
      mask -= 1 << __builtin_ctz(mask);
    }
  }

  for (; i < str.len; i += 1) {
    if (str.data[i] == buf[0]) {
      CHECK_REMAINING(i);
      i += n - 1;
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

  isize i;
  for (i = 0; ((uintptr)&str.data[i] % 16 != 0) && i < str.len; i += 1) {
    // ignoring utf8 is fine here, at least as long as we only support ASCII
    char  v =  str.data[i];
    char *c = &out.data[i];
    if ('A' <= v && v <= 'Z') {
      *c = v + 'a' - 'A';
    } else {
      *c = v;
    }
  }

  __m128i vec_a = _mm_set1_epi8('A' -  1 );
  __m128i vec_z = _mm_set1_epi8('Z' +  1 );
  __m128i shift = _mm_set1_epi8('a' - 'A');
  __m128i ones  = _mm_set1_epi8(0xFF);

  for (; i + 16 < str.len; i += 16) {
    __m128i v        = _mm_load_si128((__m128i *)&str.data[i]);
    __m128i cmp_a    = _mm_cmpgt_epi8(v, vec_a); // ( v  > 'A' - 1)
    __m128i cmp_z    = _mm_cmpgt_epi8(vec_z, v); // ('Z' >  v  + 1)

    __m128i mask     = _mm_and_si128(cmp_a, cmp_z);
    __m128i inv_mask = _mm_andnot_si128(mask, ones);

    __m128i lower    = _mm_and_si128(_mm_add_epi8(v, shift), mask);
    __m128i upper    = _mm_and_si128(v, inv_mask);

    __m128i result   = _mm_or_si128(upper, lower);
    _mm_storeu_si128((__m128i *)&out.data[i], result);
  }

  for (; i < str.len; i += 1) {
    char  v =  str.data[i];
    char *c = &out.data[i];
    if ('A' <= v && v <= 'Z') {
      *c = v + 'a' - 'A';
    } else {
      *c = v;
    }
  }

  return transmute(String, out);
}

extern String string_to_upper(String str, Allocator allocator) {
  Slice(char) out = slice_make(type_of(out), str.len, allocator);

  isize i;
  for (i = 0; ((uintptr)&str.data[i] % 16 != 0) && i < str.len; i += 1) {
    // ignoring utf8 is fine here, at least as long as we only support ASCII
    char  v =  str.data[i];
    char *c = &out.data[i];
    if ('a' <= v && v <= 'z') {
      *c = v + 'A' - 'a';
    } else {
      *c = v;
    }
  }

  __m128i vec_a = _mm_set1_epi8('a' -  1 );
  __m128i vec_z = _mm_set1_epi8('z' +  1 );
  __m128i shift = _mm_set1_epi8('A' - 'a');
  __m128i ones  = _mm_set1_epi8(0xFF);

  for (; i + 16 < str.len; i += 16) {
    __m128i v        = _mm_load_si128((__m128i *)&str.data[i]);
    __m128i cmp_a    = _mm_cmpgt_epi8(v, vec_a); // ( v  > 'a' - 1)
    __m128i cmp_z    = _mm_cmpgt_epi8(vec_z, v); // ('z' >  v  + 1)

    __m128i mask     = _mm_and_si128(cmp_a, cmp_z);
    __m128i inv_mask = _mm_andnot_si128(mask, ones);

    __m128i upper    = _mm_and_si128(_mm_add_epi8(v, shift), mask);
    __m128i lower    = _mm_and_si128(v, inv_mask);

    __m128i result   = _mm_or_si128(upper, lower);
    _mm_storeu_si128((__m128i *)&out.data[i], result);
  }

  for (; i < str.len; i += 1) {
    char  v =  str.data[i];
    char *c = &out.data[i];
    if ('a' <= v && v <= 'z') {
      *c = v + 'A' - 'a';
    } else {
      *c = v;
    }
  }

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
  return string_equal(slice_range(str, 0, prefix.len), prefix);
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
  vector_append(b, data);
  return 1;
}

extern isize builder_write_string(Builder *b, String data) {
  return builder_write_bytes(b, string_to_bytes(data));
}

extern isize builder_write_cstring(Builder *b, cstring data) {
  return builder_write_string(b, cstring_to_string(data));
}
