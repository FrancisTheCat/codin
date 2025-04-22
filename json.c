#include "json.h"

#include "fmt.h"
#include "strings.h"
#include "unicode.h"

#undef  STRING_SWITCH
#define STRING_SWITCH(s) { String _string_switch_string = (s); {
#define STRING_CASEC(str) STRING_CASE(LIT((str)))
#define STRING_DEFAULT() } else {
#define STRING_SWITCH_END() }}

internal void skip_whitespace(Json_Parser *parser) {
  while (
    parser->current < parser->data.len &&
    rune_is_whitespace(utf8_rune_at(parser->data, parser->current, nil))
  ) {
    switch (IDX(parser->data, parser->current)) {
    CASE '\n':
      parser->line  += 1;
      parser->column = 1;
    DEFAULT:
      parser->column += 1;
    }
    parser->current += 1;
  }
}

extern Json_Status json_parser_skip(Json_Parser *parser) {
  if (parser->error.kind) {
    return Json_Status_Error;
  }
  if (parser->value.kind != Json_Value_Array && parser->value.kind != Json_Value_Object) {
    return Json_Status_Continue;
  }
  Json_Status status;
  isize target_depth = parser->depth - 1;
  for (
    status =  json_parser_advance(parser);
    status != Json_Status_Error;
    status =  json_parser_advance(parser)
  ) {
    if (parser->depth == target_depth) {
      return Json_Status_Continue;
    }
  }
  return status;
}

extern Json_Status json_parser_advance(Json_Parser *parser) {
  if (parser->error.kind) {
    return Json_Status_Error;
  }
  skip_whitespace(parser);
  if (parser->current >= parser->data.len) {
    parser->error.kind = Json_Error_Unexpected_EOF;
    return Json_Status_Error;
  }

  if (utf8_rune_at(parser->data, parser->current, nil) == '"') {
    parser->current += 1;
    isize start = parser->current;
    while (
      parser->current < parser->data.len &&
      utf8_rune_at(parser->data, parser->current, nil) != '"'
    ) {
      parser->current += 1;
    }
    String str = slice_range(parser->data, start, parser->current);

    parser->current += 1;
    skip_whitespace(parser);

    if (parser->current >= parser->data.len) {
      parser->error.kind = Json_Error_Unexpected_EOF;
      return Json_Status_Error;
    }

    if (utf8_rune_at(parser->data, parser->current, nil) != ':') {
      if (parser->current < parser->data.len) {
        if (utf8_rune_at(parser->data, parser->current, nil) == ',') {
          parser->current += 1;
        }
      }

      parser->value.string = str;
      parser->value.kind   = Json_Value_String;
      return Json_Status_Continue;
    }

    parser->name = str;
    parser->current += 1;
    skip_whitespace(parser);
  }
  
  if (parser->current >= parser->data.len) {
    parser->error.kind = Json_Error_Unexpected_EOF;
    return Json_Status_Error;
  }

  switch (utf8_rune_at(parser->data, parser->current, nil)) {
  CASE '"':
    parser->current += 1;
    isize start = parser->current;
    while (
      parser->current < parser->data.len &&
      utf8_rune_at(parser->data, parser->current, nil) != '"'
    ) {
      parser->current += 1;
    }
    parser->value.string = slice_range(parser->data, start, parser->current);
    parser->value.kind   = Json_Value_String;
    parser->current     += 1;
  CASE '{':
    parser->depth     += 1;
    parser->current   += 1;
    parser->value.kind = Json_Value_Object;
  CASE '[':
    parser->depth     += 1;
    parser->current   += 1;
    parser->value.kind = Json_Value_Array;
  CASE ']':
  case '}':
    parser->current += 1;
    parser->depth   -= 1;

    skip_whitespace(parser);
    if (parser->current < parser->data.len) {
      if (utf8_rune_at(parser->data, parser->current, nil) == ',') {
        parser->current += 1;
      }
    }
    
    return Json_Status_End;

  CASE '-':
  case '0' ... '9': {
    rune  r        = utf8_rune_at(parser->data, parser->current, nil);
    b8    negative = false;
    if (r == '-') {
      negative = true;
      parser->current += 1;
      r = utf8_rune_at(parser->data, parser->current, nil);
    }
    isize integer = 0;
    for (; '0' <= r && r <= '9'; r = utf8_rune_at(parser->data, parser->current, nil)) {
      integer         *= 10;
      integer         += r - '0';
      parser->current += 1;
    }
    if (parser->current >= parser->data.len) {
      parser->error.kind = Json_Error_Unexpected_EOF;
      return Json_Status_Error;
    }
    parser->value.kind    = Json_Value_Number;
    parser->value.integer = negative ? -integer : integer;
    parser->value.number  = negative ? -integer : integer;
    if (r == '.') {
      parser->current += 1;
      f64 decimal = integer;
      f64 weight  = 1;
      r = utf8_rune_at(parser->data, parser->current, nil);
      for (; '0' <= r && r <= '9'; r = utf8_rune_at(parser->data, parser->current, nil)) {
        weight          *= 0.1f;
        decimal         += weight * (r - '0');
        parser->current += 1;
      }

      parser->value.number = negative ? -decimal : decimal;
    }

    if (r == 'e' || r == 'E') {
      b8 exponent_negative = false;
      isize exponent = 0;

      parser->current += 1;
      r = utf8_rune_at(parser->data, parser->current, nil);
      if (r == '+') {
        parser->current += 1;
        r = utf8_rune_at(parser->data, parser->current, nil);
      } else if (r == '-') {
        exponent_negative = true;
        parser->current += 1;
        r = utf8_rune_at(parser->data, parser->current, nil);
      }

      for (; '0' <= r && r <= '9'; r = utf8_rune_at(parser->data, parser->current, nil)) {
        exponent        *= 10;
        exponent        += r - '0';
        parser->current += 1;
      }

      if (exponent_negative) {
        for_range(i, 0, exponent) {
          parser->value.integer /= 10;
          parser->value.number  *= 0.1f;
        }
      } else {
        for_range(i, 0, exponent) {
          parser->value.integer *= 10;
          parser->value.number  *= 10;
        }
      }
    }
  }

  DEFAULT:
    String str = slice_start(parser->data, parser->current);
    if (string_has_prefix(str, LIT("true"))) {
      parser->current      += LIT("true").len;
      parser->value.kind    = Json_Value_Bool;
      parser->value.boolean = true;
    } else if (string_has_prefix(str, LIT("false"))) {
      parser->current      += LIT("false").len;
      parser->value.kind    = Json_Value_Bool;
      parser->value.boolean = false;
    } else if (string_has_prefix(str, LIT("null"))) {
      parser->current      += LIT("null").len;
      parser->value.kind    = Json_Value_Null;
    } else {
      parser->error.kind = Json_Error_Unexpected_Token;
      return Json_Status_Error;
    }
  }

  skip_whitespace(parser);
  if (parser->current >= parser->data.len) {
    parser->error.kind = Json_Error_Unexpected_EOF;
    return Json_Status_Error;
  }
  if (utf8_rune_at(parser->data, parser->current, nil) == ',') {
    parser->current += 1;
  }
  
  return Json_Status_Continue;
}

extern Json_Status json_parser_init(Json_Parser *parser, String data, Allocator allocator) {
  (void)allocator;

  *parser = (Json_Parser) {
    .data   = data,
    .line   = 1,
    .column = 1,
  };

  return json_parser_advance(parser);
}

extern void json_parser_destroy(Json_Parser const *parser) {
  (void)parser;
}

extern String json_value_to_string(Json_Value const *value, Allocator allocator) {
  switch (value->kind) {
  case Json_Value_Object:
    return string_clone(LIT("{}"), allocator);
  case Json_Value_String:
    return fmt_aprintfc(allocator, "\"%S\"", value->string);
  case Json_Value_Number:
    return fmt_aprintfc(allocator, "%f", value->number);
  case Json_Value_Array:
    return string_clone(LIT("[]"), allocator);
  case Json_Value_Bool:
    if (value->boolean) {
      return string_clone(LIT("true"), allocator);
    } else {
      return string_clone(LIT("false"), allocator);
    }
  case Json_Value_Null:
    return string_clone(LIT("null"), allocator);
  default:
    return (String) {0};
  }
}
