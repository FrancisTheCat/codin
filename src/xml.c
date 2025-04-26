#include "xml.h"
#include "strings.h"

internal bool _xml_rune_is_whitespace(rune r) {
  switch (r) {
  case '\r':
  case '\n':
  case '\t':
  case ' ':
    return true;
  }
  return false;
}

extern isize xml_parse_object(String data, XML_Object *out, Allocator allocator) {
  *out = (XML_Object){0};

  if (data.data[0] != '<') {
    return -1;
  }

  isize current = 1;
  while (current < data.len) {
    if (rune_is_alpha(data.data[current])) {
      current += 1;
    } else if (_xml_rune_is_whitespace(data.data[current]) || data.data[current] == '>') {
      break;
    } else {
      return -1;
    }
  }

  out->type = slice_range(data, 1, current);

  Vector(XML_Property) properties;
  vector_init(&properties, 0, 8, allocator);

  XML_Property prop;

  while (current < data.len) {
    while (_xml_rune_is_whitespace(data.data[current])) {
      current += 1;
    }
    if (data.data[current] == '/') {
      current += 1;
      if (data.data[current] != '>') {
        return -1;
      }

      out->properties = vector_to_slice(type_of(out->properties), properties);
      return current + 1;
    }
    if (data.data[current] == '>') {
      current += 1;
      break;
    }
    if (!rune_is_alpha(data.data[current])) {
      return -1;
    }

    isize start = current;
    while (current < data.len) {
      if (_xml_rune_is_whitespace(data.data[current]) || data.data[current] == '=') {
        break;
      }
      if (!(rune_is_alpha(data.data[current]) || data.data[current] == '_' || data.data[current] == '-')) {
        return -1;
      }
      current += 1;
    }

    prop.key = slice_range(data, start, current);

    while (_xml_rune_is_whitespace(data.data[current])) {
      current += 1;
    }

    if (data.data[current] != '=') {
      return -1;
    }
    current += 1;

    while (_xml_rune_is_whitespace(data.data[current])) {
      current += 1;
    }

    if (data.data[current] != '"') {
      return -1;
    }
    current += 1;

    start = current;
    while (current < data.len) {
      if (data.data[current] == '"') {
        break;
      }
      current += 1;
    }
    prop.value = slice_range(data, start, current);
    current += 1;

    vector_append(&properties, prop);
  }

  out->properties = vector_to_slice(type_of(out->properties), properties);

  Vector(XML_Object) children;
  vector_init(&children, 0, 8, allocator);

  XML_Object child;

  while (current < data.len) {
    while (_xml_rune_is_whitespace(data.data[current])) {
      current += 1;
    }
    if (data.data[current] == '<') {
      if (data.data[current + 1] == '/') {
        out->children = vector_to_slice(type_of(out->children), children);
        return current + string_index_byte(slice_start(data, current), '>') + 1;
      } else if (data.data[current + 1] == '!') {
        current += string_index_byte(slice_start(data, current), '>') + 1;
      } else {
        isize n = xml_parse_object(slice_start(data, current), &child, allocator);
        if (n < 0) {
          return -1;
        }
        current += n;
        vector_append(&children, child);
      }
    } else {
      isize n   = string_index_byte(slice_start(data, current), '<');
      out->body = slice_range(data, current, current + n);
      current  += n;
    }
  }

  return -1;
}

extern bool xml_parse_file(String data, XML_Object *out, Allocator allocator) {
  int current = 0;
  while (_xml_rune_is_whitespace(data.data[current])) {
    current += 1;
  }
  if (data.data[current] != '<') {
    return false;
  }
  if (data.data[current + 1] == '?') {
    current += 2;
    while (data.data[current] != '?') {
      current += 1;
    }
    current += 1;
    if (data.data[current] != '>') {
      return false;
    }
    current += 1;
    while (_xml_rune_is_whitespace(data.data[current])) {
      current += 1;
    }
  }
  return xml_parse_object(slice_start(data, current), out, allocator) != -1;
}

extern String xml_get_property(XML_Object const *xml, String key) {
  slice_iter(xml->properties, p, _i, {
    if (string_equal(p->key, key)) {
      return p->value;
    }
  })
  return (String) {0};
}
