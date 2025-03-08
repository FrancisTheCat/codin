#pragma once

#include "codin.h"

typedef struct {
  String key;
  String value;
} XML_Property;

typedef struct _Xml_Object {
  String                    type;
  Slice(XML_Property)       properties;
  String                    body;
  Slice(struct _Xml_Object) children;
} XML_Object;

extern isize  xml_parse_object(String data, XML_Object *out, Allocator allocator);
extern b8     xml_parse_file(String data, XML_Object *out, Allocator allocator);
extern String xml_get_property(XML_Object const *xml, String key);
