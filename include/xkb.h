#pragma once

#include "codin.h"

typedef i32 Keycode;

#define KEYS_X                                                                 \
  X( Key_Null, "" )                                                            \
                                                                               \
  X( Key_0, "0" )                                                              \
  X( Key_1, "1" )                                                              \
  X( Key_2, "2" )                                                              \
  X( Key_3, "3" )                                                              \
  X( Key_4, "4" )                                                              \
  X( Key_5, "5" )                                                              \
  X( Key_6, "6" )                                                              \
  X( Key_7, "7" )                                                              \
  X( Key_8, "8" )                                                              \
  X( Key_9, "9" )                                                              \
                                                                               \
  X( Key_A, "a" )                                                              \
  X( Key_B, "b" )                                                              \
  X( Key_C, "c" )                                                              \
  X( Key_D, "d" )                                                              \
  X( Key_E, "e" )                                                              \
  X( Key_F, "f" )                                                              \
  X( Key_G, "g" )                                                              \
  X( Key_H, "h" )                                                              \
  X( Key_I, "i" )                                                              \
  X( Key_J, "j" )                                                              \
  X( Key_K, "k" )                                                              \
  X( Key_L, "l" )                                                              \
  X( Key_M, "m" )                                                              \
  X( Key_N, "n" )                                                              \
  X( Key_O, "o" )                                                              \
  X( Key_P, "p" )                                                              \
  X( Key_Q, "q" )                                                              \
  X( Key_R, "r" )                                                              \
  X( Key_S, "s" )                                                              \
  X( Key_T, "t" )                                                              \
  X( Key_U, "u" )                                                              \
  X( Key_V, "v" )                                                              \
  X( Key_W, "w" )                                                              \
  X( Key_X, "x" )                                                              \
  X( Key_Y, "y" )                                                              \
  X( Key_Z, "z" )                                                              \
                                                                               \
  X( Key_F0,  "F0"  )                                                          \
  X( Key_F1,  "F1"  )                                                          \
  X( Key_F2,  "F2"  )                                                          \
  X( Key_F3,  "F3"  )                                                          \
  X( Key_F4,  "F4"  )                                                          \
  X( Key_F5,  "F5"  )                                                          \
  X( Key_F6,  "F6"  )                                                          \
  X( Key_F7,  "F7"  )                                                          \
  X( Key_F8,  "F8"  )                                                          \
  X( Key_F9,  "F9"  )                                                          \
  X( Key_F10, "F10" )                                                          \
  X( Key_F11, "F11" )                                                          \
  X( Key_F12, "F12" )                                                          \
  X( Key_F13, "F13" )                                                          \
  X( Key_F14, "F14" )                                                          \
  X( Key_F15, "F15" )                                                          \
  X( Key_F16, "F16" )                                                          \
  X( Key_F17, "F17" )                                                          \
  X( Key_F18, "F18" )                                                          \
  X( Key_F19, "F19" )                                                          \
  X( Key_F20, "F20" )                                                          \
  X( Key_F21, "F21" )                                                          \
  X( Key_F22, "F22" )                                                          \
  X( Key_F23, "F23" )                                                          \
  X( Key_F24, "F24" )                                                          \
                                                                               \
  X( Key_Exclam,      "exclam"      )                                          \
  X( Key_At,          "at"          )                                          \
  X( Key_Hashtag,     "numbersign"  )                                          \
  X( Key_Dollar,      "dollar"      )                                          \
  X( Key_Percent,     "percent"     )                                          \
  X( Key_Caret,       "asciicircum" )                                          \
  X( Key_Ampersand,   "ampersand"   )                                          \
  X( Key_Asterisk,    "asterisk"    )                                          \
  X( Key_Paren_Left,  "parenleft"   )                                          \
  X( Key_Paren_Right, "parenright"  )                                          \
                                                                               \
  X( Key_Brace_Left,    "braceleft"    )                                       \
  X( Key_Brace_Right,   "braceright"   )                                       \
  X( Key_Bracket_Left,  "bracketleft"  )                                       \
  X( Key_Bracket_Right, "bracketright" )                                       \
                                                                               \
  X( Key_Semicolon,  "semicolon"  )                                            \
  X( Key_Colon,      "colon"      )                                            \
  X( Key_Quote,      "quotedbl"   )                                            \
  X( Key_Apostrophe, "apostrophe" )                                            \
  X( Key_Period,     "period"     )                                            \
  X( Key_Greater,    "greater"    )                                            \
  X( Key_Comma,      "comma"      )                                            \
  X( Key_Less,       "less"       )                                            \
  X( Key_Slash,      "slash"      )                                            \
  X( Key_Question,   "question"   )                                            \
  X( Key_Backslash,  "backslash"  )                                            \
  X( Key_Bar,        "bar"        )                                            \
                                                                               \
  X( Key_Plus,       "plus"       )                                            \
  X( Key_Minus,      "minus"      )                                            \
  X( Key_Equal,      "equal"      )                                            \
  X( Key_Underscore, "underscore" )                                            \
  X( Key_Grave,      "grave"      )                                            \
  X( Key_Tilde,      "asciitilde" )                                            \
                                                                               \
  X( Key_Left,      "Left"      )                                              \
  X( Key_Right,     "Right"     )                                              \
  X( Key_Up,        "Up"        )                                              \
  X( Key_Down,      "Down"      )                                              \
                                                                               \
  X( Key_Space,     "space"     )                                              \
  X( Key_Escape,    "Escape"    )                                              \
  X( Key_Return,    "Return"    )                                              \
  X( Key_Backspace, "BackSpace" )                                              \
  X( Key_Tab,       "Tab"       )                                              \
  X( Key_Caps_Lock, "Caps_Lock" )                                              \
                                                                               \
  X( Key_LShift,    "Shift_L"   )                                              \
  X( Key_RShift,    "Shift_R"   )                                              \
  X( Key_LControl,  "Control_L" )                                              \
  X( Key_RControl,  "Control_R" )                                              \
  X( Key_LAlt,      "Alt_L"     )                                              \
  X( Key_RAlt,      "Alt_R"     )                                              \
  X( Key_LSuper,    "Super_L"   )                                              \
  X( Key_RSuper,    "Super_R"   )                                              \
                                                                               \
  X( Key_MAX_VALUE, "" )                                                       \

#define X(key, _xkb_name) key,
typedef enum {
  KEYS_X
} Key;
#undef X

ENUM_TO_STRING_PROC_DECL(Key, key) {
  #define X(key, _xkb_name) case key: return LIT(#key);
  switch (key) {
    KEYS_X
  }
  #undef X
  return LIT("Key_INVALID");
}

typedef struct {
  Key  *lowercase;
  Key  *uppercase;
  isize len;
} Keymap;

#define X(key, xkb_name) [key] = LIT(xkb_name),
String key_names[] = {
    KEYS_X
};
#undef X

#define XKB_TOKENS(X)                                                          \
  X(XKB_T_Ident,          0  )                                                 \
  X(XKB_T_String,         1  )                                                 \
  X(XKB_T_Symbol,         2  )                                                 \
  X(XKB_T_Number,         3  )                                                 \
  X(XKB_T_Bang,           '!')                                                 \
  X(XKB_T_Plus,           '+')                                                 \
  X(XKB_T_Period,         '.')                                                 \
  X(XKB_T_Comma,          ',')                                                 \
  X(XKB_T_Minus,          '-')                                                 \
  X(XKB_T_Open_Paren,     '(')                                                 \
  X(XKB_T_Close_Paren,    ')')                                                 \
  X(XKB_T_Semicolon,      ';')                                                 \
  X(XKB_T_Equals,         '=')                                                 \
  X(XKB_T_Open_Square,    '[')                                                 \
  X(XKB_T_Close_Square,   ']')                                                 \
  X(XKB_T_Open_Squirly,   '{')                                                 \
  X(XKB_T_Close_Squirly,  '}')                                                 \

X_ENUM_EXPLICIT(XKB_Token_Type, XKB_TOKENS)

typedef struct {
  XKB_Token_Type type;
  String         string;
  isize          number;
} XKB_Token;

internal bool xkb_parse_key_codes(String source, Keymap *out_keymap, Allocator allocator) {
  Vector(XKB_Token) tokens;
  vector_init(&tokens, 0, 8, context.temp_allocator);

  isize start   = 0;
  isize current = 0;

  char delimiter = 0;

  XKB_Token token = {0};

  while (current < source.len) {
    start = current;
    switch (source.data[current]) {
    case '\n':
    case '\r':
    case '\t':
    case ' ':
    case 0:
      current += 1;
      break;

    case '"':
    case '<':
      delimiter = source.data[current];
      if (delimiter == '<') {
        delimiter = '>';
      }

      current += 1;

      while (source.data[current] != delimiter) {
        current += 1;
      }

      token.type   = delimiter == '>' ? XKB_T_Symbol : XKB_T_String;
      token.string = slice_range(source, start + 1, current);
      vector_append(&tokens, token);

      current += 1;

      break;

    case ',':
    case '=':
    case ';':
    case '{':
    case '}':
    case ']':
    case '[':
    case ')':
    case '(':
    case '+':
    case '-':
    case '!':
    case '.':
      token.type = (XKB_Token_Type)source.data[current];
      vector_append(&tokens, token);
      current += 1;
      break;

    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      token.number = 0;
      while ('0' <= source.data[current] && source.data[current] <= '9') {
        token.number *= 10;
        token.number += source.data[current] - '0';
        current += 1;
      }
      token.type   = XKB_T_Number;
      token.string = slice_range(source, start, current);
      vector_append(&tokens, token);
      break;

    default:
      if (rune_is_alpha(source.data[current]) || (source.data[current] == '_')) {
        while (rune_is_alpha_numeric(source.data[current]) || (source.data[current] == '_')) {
          current += 1;
        }
        token.type = XKB_T_Ident;
        token.string = slice_range(source, start, current);

        vector_append(&tokens, token);
      } else {
        log_warnf(LIT("unexpected Token: '%c' (%d)"), source.data[current], source.data[current]);
        current += 1;
      }
    }
  }

  Vector(String) keycodes;
  vector_init(&keycodes, 0, 8, context.temp_allocator);

  slice_iter(tokens, token, i, {
    if (token->type == XKB_T_Ident && string_equal(token->string, LIT("xkb_keycodes"))) {
      isize current = i + 1;

      if (tokens.data[current].type != XKB_T_String) {
        log_error(LIT("Failed to parse xkb layout"));
        return false;
      }
      current += 1;

      if (tokens.data[current].type != XKB_T_Open_Squirly) {
        log_error(LIT("Failed to parse xkb layout"));
        return false;
      }
      current += 1;

      while (current < tokens.len && tokens.data[current].type != XKB_T_Close_Squirly) {
        String name = {0};
        if (tokens.data[current].type != XKB_T_Symbol) {
          while (tokens.data[current].type != XKB_T_Semicolon) {
            current += 1;
          }
          current += 1;
          continue;
        }

        name = tokens.data[current].string;

        current += 1;
        
        if (tokens.data[current].type != XKB_T_Equals) {
          log_error(LIT("Failed to parse xkb layout"));
          return false;
        }
        current += 1;

        if (tokens.data[current].type != XKB_T_Number) {
          log_error(LIT("Failed to parse xkb layout"));
          return false;
        }
        while (keycodes.len < tokens.data[current].number) {
          vector_append(&keycodes, (String){.len = 0});
        }
        if (keycodes.len > tokens.data[current].number) {
          keycodes.data[tokens.data[current].number] = name;
        } else {
          vector_append(&keycodes, name);
        }

        current += 1;

        if (tokens.data[current].type != XKB_T_Semicolon) {
          log_error(LIT("Failed to parse xkb layout"));
          return false;
        }
        current += 1;
      }

      break;
    }
  });

  Vector(Key) lowercase;
  vector_init(&lowercase, 0, 8, allocator);
  Vector(Key) uppercase;
  vector_init(&uppercase, 0, 8, allocator);

  slice_iter(tokens, token, i, {
    if (token->type == XKB_T_Ident && string_equal(token->string, LIT("xkb_symbols"))) {
      isize current = i + 1;

      if (tokens.data[current].type != XKB_T_String) {
        log_error(LIT("Failed to parse xkb layout"));
        return false;
      }
      current += 1;

      if (tokens.data[current].type != XKB_T_Open_Squirly) {
        log_error(LIT("Failed to parse xkb layout"));
        return false;
      }
      current += 1;

      while (current < tokens.len && tokens.data[current].type != XKB_T_Close_Squirly) {
        String symbol_name = {0};
        if (tokens.data[current].type != XKB_T_Ident || !string_equal(tokens.data[current].string, LIT("key"))) {
          while (tokens.data[current].type != XKB_T_Semicolon) {
            current += 1;
          }
          current += 1;
          continue;
        }
        current += 1;
        
        if (tokens.data[current].type != XKB_T_Symbol) {
          log_error(LIT("Failed to parse xkb layout"));
          return false;
        }
        symbol_name = tokens.data[current].string;
        current += 1;

        Keycode keycode = -1;
        for_range(i, 0, keycodes.len) {
          if (string_equal(keycodes.data[i], symbol_name)) {
            keycode = i;
            break;
          }
        }

        if (tokens.data[current].type != XKB_T_Open_Squirly) {
          log_error(LIT("Failed to parse xkb layout"));
          return false;
        }
        current += 1;

        if (tokens.data[current].type != XKB_T_Open_Square) {
          while (!(
            tokens.data[current].type == XKB_T_Ident &&
            string_equal(tokens.data[current].string, LIT("symbols")))
          ) {
            current += 1;
          }
          current += 5;
        }
        current += 1;

        if (tokens.data[current].type != XKB_T_Number && tokens.data[current].type != XKB_T_Ident) {
          log_error(LIT("Failed to parse xkb layout"));
          return false;
        }
        String value = tokens.data[current].string;
        current += 1;

        Key key = Key_Null;
        for_range(k, 0, count_of(key_names)) {
          if (string_equal(key_names[k], value)) {
            key = (Key)k;
            break;
          }
        }

        while (lowercase.len < keycode) {
          vector_append(&lowercase, Key_Null);
        }
        if (lowercase.len > keycode) {
          lowercase.data[key] = key;
        } else {
          vector_append(&lowercase, key);
        }

        if (tokens.data[current].type == XKB_T_Comma) {
          current += 1;
          value = tokens.data[current].string;
          current += 1;

          key = Key_Null;
          for_range(k, 0, count_of(key_names)) {
            if (string_equal(key_names[k], value)) {
              key = (Key)k;
              break;
            }
          }

          while (uppercase.len < keycode) {
            vector_append(&uppercase, Key_Null);
          }
          if (uppercase.len > keycode) {
            uppercase.data[key] = key;
          } else {
            vector_append(&uppercase, key);
          }
        }

        current += 1;

        while (tokens.data[current].type != XKB_T_Semicolon) {
          current += 1;
        }
        current += 1;
      }

      break;
    }
  });

  vector_reserve(&uppercase, lowercase.len);
  uppercase.len = uppercase.len;
  vector_iter(uppercase, uk, i, {
    if (*uk == Key_Null) {
      *uk = lowercase.data[i];
    }
  })

  out_keymap->lowercase = lowercase.data;
  out_keymap->uppercase = uppercase.data;
  out_keymap->len       = lowercase.len;

  return true;
}

internal void xkb_keymap_destroy(Keymap const* km, Allocator allocator) {
  mem_free(km->lowercase, km->len, allocator);
  mem_free(km->uppercase, km->len, allocator);
}
