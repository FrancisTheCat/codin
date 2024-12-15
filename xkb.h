#include "codin.h"

typedef i32 Keycode;

typedef enum {
  Key_Null = 0,

  Key_0 = '0',
  Key_1,
  Key_2,
  Key_3,
  Key_4,
  Key_5,
  Key_6,
  Key_7,
  Key_8,
  Key_9,

  Key_A = 'A',
  Key_B,
  Key_C,
  Key_D,
  Key_E,
  Key_F,
  Key_G,
  Key_H,
  Key_I,
  Key_J,
  Key_K,
  Key_L,
  Key_M,
  Key_N,
  Key_O,
  Key_P,
  Key_Q,
  Key_R,
  Key_S,
  Key_T,
  Key_U,
  Key_V,
  Key_W,
  Key_X,
  Key_Y,
  Key_Z,

  Key_F0,
  Key_F1,
  Key_F2,
  Key_F3,
  Key_F4,
  Key_F5,
  Key_F6,
  Key_F7,
  Key_F8,
  Key_F9,
  Key_F10,
  Key_F11,
  Key_F12,
  Key_F13,
  Key_F14,
  Key_F15,
  Key_F16,
  Key_F17,
  Key_F18,
  Key_F19,
  Key_F20,
  Key_F21,
  Key_F22,
  Key_F23,
  Key_F24,

  Key_Comma,
  Key_Period,
  Key_Slash,
  Key_Semicolon,
  Key_Quote,

  Key_Left,
  Key_Right,
  Key_Up,
  Key_Down,

  Key_Space,
  Key_Escape,
  Key_Return,
  Key_Backspace,
  Key_Tab,
  Key_Caps_Lock,

  Key_LShift,
  Key_RShift,
  Key_LControl,
  Key_RControl,
  Key_LAlt,
  Key_RAlt,
  Key_LSuper,
  Key_RSuper,

  Key_MAX_VALUE,
} Key;

internal String get_key_name(Key key) {
  switch (key) {
  case Key_Null:
    return LIT("Key_Null");
  case Key_0:
    return LIT("Key_0");
  case Key_1:
    return LIT("Key_1");
  case Key_2:
    return LIT("Key_2");
  case Key_3:
    return LIT("Key_3");
  case Key_4:
    return LIT("Key_4");
  case Key_5:
    return LIT("Key_5");
  case Key_6:
    return LIT("Key_6");
  case Key_7:
    return LIT("Key_7");
  case Key_8:
    return LIT("Key_8");
  case Key_9:
    return LIT("Key_9");
  case Key_F0:
    return LIT("Key_F0");
  case Key_F1:
    return LIT("Key_F1");
  case Key_F2:
    return LIT("Key_F2");
  case Key_F3:
    return LIT("Key_F3");
  case Key_F4:
    return LIT("Key_F4");
  case Key_F5:
    return LIT("Key_F5");
  case Key_F6:
    return LIT("Key_F6");
  case Key_F7:
    return LIT("Key_F7");
  case Key_F8:
    return LIT("Key_F8");
  case Key_F9:
    return LIT("Key_F9");
  case Key_F10:
    return LIT("Key_F10");
  case Key_F11:
    return LIT("Key_F11");
  case Key_F12:
    return LIT("Key_F12");
  case Key_F13:
    return LIT("Key_F13");
  case Key_F14:
    return LIT("Key_F14");
  case Key_F15:
    return LIT("Key_F15");
  case Key_F16:
    return LIT("Key_F16");
  case Key_F17:
    return LIT("Key_F17");
  case Key_F18:
    return LIT("Key_F18");
  case Key_F19:
    return LIT("Key_F19");
  case Key_F20:
    return LIT("Key_F20");
  case Key_F21:
    return LIT("Key_F21");
  case Key_F22:
    return LIT("Key_F22");
  case Key_F23:
    return LIT("Key_F23");
  case Key_F24:
    return LIT("Key_F24");
  case Key_A:
    return LIT("Key_A");
  case Key_B:
    return LIT("Key_B");
  case Key_C:
    return LIT("Key_C");
  case Key_D:
    return LIT("Key_D");
  case Key_E:
    return LIT("Key_E");
  case Key_F:
    return LIT("Key_F");
  case Key_G:
    return LIT("Key_G");
  case Key_H:
    return LIT("Key_H");
  case Key_I:
    return LIT("Key_I");
  case Key_J:
    return LIT("Key_J");
  case Key_K:
    return LIT("Key_K");
  case Key_L:
    return LIT("Key_L");
  case Key_M:
    return LIT("Key_M");
  case Key_N:
    return LIT("Key_N");
  case Key_O:
    return LIT("Key_O");
  case Key_P:
    return LIT("Key_P");
  case Key_Q:
    return LIT("Key_Q");
  case Key_R:
    return LIT("Key_R");
  case Key_S:
    return LIT("Key_S");
  case Key_T:
    return LIT("Key_T");
  case Key_U:
    return LIT("Key_U");
  case Key_V:
    return LIT("Key_V");
  case Key_W:
    return LIT("Key_W");
  case Key_X:
    return LIT("Key_X");
  case Key_Y:
    return LIT("Key_Y");
  case Key_Z:
    return LIT("Key_Z");
  case Key_Space:
    return LIT("Key_Space");
  case Key_Escape:
    return LIT("Key_Escape");
  case Key_Return:;
    return LIT("Key_Return");
  case Key_Backspace:
    return LIT("Key_Backspace");
  case Key_Tab:
    return LIT("Key_Tab");
  case Key_Caps_Lock:
    return LIT("Key_Caps_Lock");
  case Key_LShift:
    return LIT("Key_LShift");
  case Key_RShift:
    return LIT("Key_RShift");
  case Key_LControl:
    return LIT("Key_LControl");
  case Key_RControl:
    return LIT("Key_RControl");
  case Key_LAlt:
    return LIT("Key_LAlt");
  case Key_RAlt:
    return LIT("Key_RAlt");
  case Key_LSuper:
    return LIT("Key_LSuper");
  case Key_RSuper:
    return LIT("Key_RSuper");
  case Key_Comma:
    return LIT("Key_Comma");
  case Key_Period:
    return LIT("Key_Period");
  case Key_Slash:
    return LIT("Key_Slash");
  case Key_Semicolon:
    return LIT("Key_Semicolon");
  case Key_Quote:
    return LIT("Key_Quote");
  case Key_Left:
    return LIT("Key_Left");
  case Key_Right:
    return LIT("Key_Right");
  case Key_Up:
    return LIT("Key_Up");
  case Key_Down:
    return LIT("Key_Down");
  case Key_MAX_VALUE:
    return LIT("Key_MAX_VALUE");
    break;
  }

  return LIT("Key_<Invalid>");
}

typedef Slice(Key) Keymap;

struct {
  Key     key;
  cstring name;
} key_names[] = {
  { Key_1, "1" },
  { Key_2, "2" },
  { Key_3, "3" },
  { Key_4, "4" },
  { Key_5, "5" },
  { Key_6, "6" },
  { Key_7, "7" },
  { Key_8, "8" },
  { Key_9, "9" },
  { Key_0, "0" },

  { Key_F1,   "F1"  },
  { Key_F2,   "F2"  },
  { Key_F3,   "F3"  },
  { Key_F4,   "F4"  },
  { Key_F5,   "F5"  },
  { Key_F6,   "F6"  },
  { Key_F7,   "F7"  },
  { Key_F8,   "F8"  },
  { Key_F9,   "F9"  },
  { Key_F10,  "F10" },
  { Key_F11,  "F11" },
  { Key_F12,  "F12" },
  { Key_F13,  "F13" },
  { Key_F14,  "F14" },
  { Key_F15,  "F15" },
  { Key_F16,  "F16" },
  { Key_F17,  "F17" },
  { Key_F18,  "F18" },
  { Key_F19,  "F19" },
  { Key_F20,  "F20" },
  { Key_F21,  "F21" },
  { Key_F22,  "F22" },
  { Key_F23,  "F23" },
  { Key_F24,  "F24" },

  { Key_A, "a" },
  { Key_B, "b" },
  { Key_C, "c" },
  { Key_D, "d" },
  { Key_E, "e" },
  { Key_F, "f" },
  { Key_G, "g" },
  { Key_H, "h" },
  { Key_I, "i" },
  { Key_J, "j" },
  { Key_K, "k" },
  { Key_L, "l" },
  { Key_M, "m" },
  { Key_N, "n" },
  { Key_O, "o" },
  { Key_P, "p" },
  { Key_Q, "q" },
  { Key_R, "r" },
  { Key_S, "s" },
  { Key_T, "t" },
  { Key_U, "u" },
  { Key_V, "v" },
  { Key_W, "w" },
  { Key_X, "x" },
  { Key_Y, "y" },
  { Key_Z, "z" },

  { Key_Semicolon, "semicolon" },
  { Key_Comma,     "comma"     },
  { Key_Period,    "period"    },
  { Key_Slash,     "slash"     },

  { Key_Left,      "Left"      },
  { Key_Right,     "Right"     },
  { Key_Up,        "Up"        },
  { Key_Down,      "Down"      },

  { Key_Space,     "space"     },
  { Key_Escape,    "Escape"    },
  { Key_Return,    "Return"    },
  { Key_Backspace, "BackSpace" },
  { Key_Tab,       "Tab"       },
  { Key_Caps_Lock, "Caps_Lock" },

  { Key_LShift,    "Shift_L"   },
  { Key_RShift,    "Shift_R"   },
  { Key_LControl,  "Control_L" },
  { Key_RControl,  "Control_R" },
  { Key_LAlt,      "Alt_L"     },
  { Key_RAlt,      "Alt_R"     },
  { Key_LSuper,    "Super_L"   },
  { Key_RSuper,    "Super_R"   },
};

typedef enum {
  XKB_T_Ident,
  XKB_T_String,
  XKB_T_Symbol,
  XKB_T_Number,
  XKB_T_Period        = '.',
  XKB_T_Plus          = '+',
  XKB_T_Minus         = '-',
  XKB_T_Bang          = '!',
  XKB_T_Comma         = ',',
  XKB_T_Equals        = '=',
  XKB_T_Semicolon     = ';',
  XKB_T_Open_Squirly  = '{',
  XKB_T_Close_Squirly = '}',
  XKB_T_Open_Square   = '[',
  XKB_T_Close_Square  = ']',
  XKB_T_Open_Paren    = '(',
  XKB_T_Close_Paren   = ')',
} XKB_Token_Type;

typedef struct {
  XKB_Token_Type type;
  String         string;
  isize          number;
} XKB_Token;

internal b8 parse_key_codes(String source, Keymap *out_keymap, Allocator allocator) {
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

  Vector(Key) keymap;
  vector_init(&keymap, 0, 8, allocator);

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

        if (tokens.data[current].type != XKB_T_Open_Squirly) {
          log_error(LIT("Failed to parse xkb layout"));
          return false;
        }
        current += 1;

        while (tokens.data[current].type != XKB_T_Open_Square) {
          current += 1;
        }
        current += 1;

        if (tokens.data[current].type != XKB_T_Number && tokens.data[current].type != XKB_T_Ident) {
          log_error(LIT("Failed to parse xkb layout"));
          return false;
        }
        String value = tokens.data[current].string;
        current += 1;

        Key key = Key_Null;
        for_range(i, 0, count_of(key_names)) {
          type_of(key_names[i]) name = key_names[i];
          if (string_equal(cstring_to_string(name.name), value)) {
            key = name.key;
            break;
          }
        }

        Keycode keycode = -1;
        for_range(i, 0, keycodes.len) {
          if (string_equal(keycodes.data[i], symbol_name)) {
            keycode = i;
            break;
          }
        }

        while (keymap.len < keycode) {
          vector_append(&keymap, Key_Null);
        }
        if (keymap.len > keycode) {
          keymap.data[key] = key;
        } else {
          vector_append(&keymap, key);
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

  *out_keymap = vector_to_slice(Keymap, keymap);

  return false;
}
