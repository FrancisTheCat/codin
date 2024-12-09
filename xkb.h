#include "codin.h"

typedef i32 Keycode;

typedef Vector(Keycode) Keymap;

typedef enum {
  XKB_T_Ident,
  XKB_T_String,
  XKB_T_Number,
  XKB_T_Comma         = ',',
  XKB_T_Equals        = '=',
  XKB_T_Semicolon     = ';',
  XKB_T_Open_Squirly  = '{',
  XKB_T_Close_Squirly = '}',
  XKB_T_Open_Square   = ']',
  XKB_T_Close_Square  = '[',
} XKB_Token_Type;

typedef struct {
  XKB_Token_Type type;
  union {
    String string;
    isize  number;
  } data;
} XKB_Token;

typedef struct {
  Slice(XKB_Token) tokens;
} XKB_Parser;

internal b8 parse_key_codes(String source, Keymap *keymap, Allocator allocator) {
  Vector(XKB_Token) tokens;

  isize start   = 0;
  isize current = 0;

  while (current < source.len) {
    switch (source.data[current]) {
    case ',':
      break;
    case '=':
      break;
    case ';':
      break;
    case '{':
      break;
    case '}':
      break;
    case ']':
      break;
    case '[':
      break;
    }
  }

  return false;
}
