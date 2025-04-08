#pragma once

#include "codin.h"

typedef enum {
  Json_Value_Null = 0,
  Json_Value_Object,
  Json_Value_String,
  Json_Value_Number,
  Json_Value_Array,
  Json_Value_Bool,
} Json_Value_Kind;

typedef struct {
  union {
    struct {
      i64 integer;
      f64 number;
    };
    String string;
    b8     boolean;
  };
  Json_Value_Kind kind;
} Json_Value;

typedef enum {
  Json_Error_None = 0,
  Json_Error_Unexpected_Token,
  Json_Error_Unexpected_EOF,
  Json_Error_Unterminated_String,
  Json_Error_Duplicated_Field,
  Json_Error_OOM,
} Json_Error_Kind;

typedef enum {
  Json_Status_Continue = 0,
  Json_Status_Error,
  Json_Status_End,
} Json_Status;

typedef struct {
  isize  current, line, column, depth;
  struct {
    isize line, column;
    Json_Error_Kind kind;
  } error;
  String data, name;
  Json_Value value;
} Json_Parser;

extern Json_Status json_parser_init(Json_Parser *parser, String data, Allocator allocator);
extern void        json_parser_destroy(Json_Parser const *parser);
extern Json_Status json_parser_advance(Json_Parser *parser);
extern Json_Status json_parser_skip(Json_Parser *parser);
extern String      json_value_to_string(Json_Value const *value, Allocator allocator);
