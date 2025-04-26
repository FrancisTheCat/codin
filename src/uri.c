#include "uri.h"

#include "base64.h"
#include "bit_array.h"
#include "strings.h"

internal String uri_scheme_strings[] = {
  [Uri_Scheme_Data] = LIT("data"),
  [Uri_Scheme_File] = LIT("file"),
};

#define array_iter_v(array, elem, i, BLOCK...)                                 \
  for (isize i = 0; i < count_of(array); i++) {                                \
    type_of((array)[0]) elem = (array)[i];                                     \
    { BLOCK; }                                                                 \
  }

extern Uri_Scheme uri_get_scheme(String uri) {
  Uri_Scheme scheme = Uri_Scheme_Unknown;

  isize colon_offset = string_index_byte(uri, ':');
  if (colon_offset < 0) {
    return scheme;
  }
  String scheme_str = slice_end(uri, colon_offset);

  array_iter_v(uri_scheme_strings, str, i, {
    if (string_equal(scheme_str, str)) {
      scheme = i;
      break;
    }
  });

  return scheme;
}

extern bool uri_file_parse(String str, Uri_File *uri) {
  *uri = (Uri_File) {0};
  if (uri_get_scheme(str) != Uri_Scheme_File) {
    return false;
  }
  str = slice_start(str, uri_scheme_strings[Uri_Scheme_File].len + 1);

  if (str.len < 2) {
    return false;
  }

  if (IDX(str, 0) != '/') {
    return false;
  }

  str = slice_start(str, 1);

  if (IDX(str, 0) == '/') {
    str = slice_start(str, 1);

    isize host_len = string_index_byte(str, '/');
    if (host_len < 0) {
      return false;
    }

    uri->host = slice_end(str, host_len);

    str = slice_start(str, host_len + 1);
  }

  if (!uri->host.len) {
    uri->host = LIT("localhost");
  }

  uri->path = str;

  return true;
}

extern bool uri_data_parse(String str, Uri_Data *uri, Allocator allocator) {
  *uri = (Uri_Data) {0};
  if (uri_get_scheme(str) != Uri_Scheme_Data) {
    return false;
  }

  str = slice_start(str, uri_scheme_strings[Uri_Scheme_Data].len + 1);

  isize attributes_len = string_index_byte(str, ',');

  if (attributes_len < 0) {
    return false;
  }

  String attributes_string = slice_end(str, attributes_len);

  Vector(Uri_Data_Attribute) attributes;
  vector_init(&attributes, 0, 8, allocator);

  bool first = true;
  while (attributes_string.len > 0) {
    isize attribute_len = string_index_byte(attributes_string, ';');
    String attribute_string;
    if (attribute_len == -1) {
      attribute_string = attributes_string;
    } else {
      attribute_string = slice_end(attributes_string, attribute_len);
    }

    attributes_string = slice_start(attributes_string, attribute_string.len + 1);

    if (string_equal(attribute_string, LIT("base64"))) {
      if (attributes_string.len > 0) {
        vector_delete(attributes);
        return false;
      }
      uri->base_64 = true;
      break;
    }

    if (first) {
      isize slash_offset = string_index_byte(attribute_string, '/');
      if (slash_offset != -1) {
        if (uri->media_type.len) {
          vector_delete(attributes);
          return false;
        }
        uri->media_type = attribute_string;
        continue;
      }
    }
    isize equals_offset = string_index_byte(attribute_string, '=');
    if (equals_offset < 0) {
      vector_delete(attributes);
      return false;
    }
    
    Uri_Data_Attribute attribute = {
      .key   = slice_end(attribute_string,   equals_offset    ),
      .value = slice_start(attribute_string, equals_offset + 1),
    };
    vector_append(&attributes, attribute);

    first = false;
  }

  String data_string = slice_start(str, attributes_len + 1);

  if (uri->base_64) {
    if (!base64_decode(data_string, &uri->data, allocator)) {
      vector_delete(attributes);
      return false;
    };
  } else {
    uri->data = string_to_bytes(data_string);
  }

  return true;
}

extern void uri_data_destroy(Uri_Data const *uri, Allocator allocator) {
  slice_delete(uri->attributes, allocator);
  if (uri->base_64) {
    slice_delete(uri->data, allocator);
  }
}
