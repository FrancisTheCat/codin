#pragma once

#include "codin.h"

typedef enum {
  Uri_Scheme_Unknown = 0,
  Uri_Scheme_Data,
  Uri_Scheme_File,
} Uri_Scheme;

typedef struct {
  String host, path;
} Uri_File;

typedef struct {
  String key, value;
} Uri_Data_Attribute;

typedef struct {
  String                    media_type;
  Slice(Uri_Data_Attribute) attributes;
  b8                        base_64;
  Byte_Slice                data;
} Uri_Data;

extern Uri_Scheme uri_get_scheme(String uri);
extern b8         uri_file_parse(String str, Uri_File *uri);
extern b8         uri_data_parse(String str, Uri_Data *uri, Allocator allocator);
extern void       uri_data_destroy(Uri_Data const *uri, Allocator allocator);
