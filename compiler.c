#include "codin.h"

#define TOKENS(X)                                                              \
  X(Token_Type_String)                                                         \
  X(Token_Type_Int)                                                            \
  X(Token_Type_Float)                                                          \
  X(Token_Type_Rune)                                                           \
  X(Token_Type_Ident)                                                          \
                                                                               \
  X(Token_Type_Assign)                                                         \
  X(Token_Type_Semicolon)                                                      \
  X(Token_Type_Colon)                                                          \
  X(Token_Type_Comma)                                                          \
  X(Token_Type_Arrow)                                                          \
  X(Token_Type_Dot)                                                            \
  X(Token_Type_Dollar)                                                         \
  X(Token_Type_Pointer)                                                        \
                                                                               \
  X(Token_Type_Plus)                                                           \
  X(Token_Type_Minus)                                                          \
  X(Token_Type_Multiply)                                                       \
  X(Token_Type_Divide)                                                         \
  X(Token_Type_Not)                                                            \
  X(Token_Type_And)                                                            \
  X(Token_Type_Or)                                                             \
  X(Token_Type_Less)                                                           \
  X(Token_Type_More)                                                           \
  X(Token_Type_Percent)                                                        \
                                                                               \
  X(Token_Type_Open_Paren)                                                     \
  X(Token_Type_Close_Paren)                                                    \
  X(Token_Type_Open_Squirly)                                                   \
  X(Token_Type_Close_Squirly)                                                  \
  X(Token_Type_Open_Square)                                                    \
  X(Token_Type_Close_Square)                                                   \
                                                                               \
  X(Token_Type_If)                                                             \
  X(Token_Type_For)                                                            \
  X(Token_Type_Var)                                                            \
  X(Token_Type_Const)                                                          \
  X(Token_Type_Break)                                                          \
  X(Token_Type_Return)                                                         \
  X(Token_Type_Defer)                                                          \
  X(Token_Type_Continue)                                                       \
  X(Token_Type_Fn)                                                             \
  X(Token_Type_Def)                                                            \
  X(Token_Type_Enum)                                                           \
  X(Token_Type_Union)                                                          \
  X(Token_Type_Struct)                                                         \
  X(Token_Type_Ignore)                                                         \
  X(Token_Type_Import)                                                         \
  X(Token_Type_Dynamic)                                                        \
                                                                               \
  X(Token_Type_EOF)                                                            \

X_ENUM(Token_Type, TOKENS);

typedef struct {
  i32    line, column;
  String file;
} Token_Location;

typedef struct {
  Token_Type     type;
  Token_Location location;
  String         lexeme;
  union {
    String string;
    isize  integer;
    rune   rune;
    f64    decimal;
  } literal;
} Token;

typedef Vector(Token) Token_Vector;

struct {
  Token_Type type;
  String string;
} keyword_strings[] = {
  { Token_Type_If,       LIT("if"),       },
  { Token_Type_For,      LIT("for"),      },
  { Token_Type_Break,    LIT("break"),    },
  { Token_Type_Return,   LIT("return"),   },
  { Token_Type_Defer,    LIT("defer"),    },
  { Token_Type_Continue, LIT("continue"), },

  { Token_Type_Fn,       LIT("fn"),       },
  { Token_Type_Def,      LIT("type"),     },
  { Token_Type_Var,      LIT("var"),      },
  { Token_Type_Enum,     LIT("enum"),     },
  { Token_Type_Const,    LIT("const"),    },
  { Token_Type_Union,    LIT("union"),    },
  { Token_Type_Struct,   LIT("struct"),   },
  { Token_Type_Ignore,   LIT("_"),        },
  { Token_Type_Import,   LIT("import"),   },
  { Token_Type_Dynamic,  LIT("dynamic"),  },
};

struct {
  Token_Type type;
  rune       c;
} single_char_tokens[] = {
  { Token_Type_Plus,      '+' },
  { Token_Type_Minus,     '-' },
  { Token_Type_Multiply,  '*' },
  { Token_Type_Divide,    '/' },
  { Token_Type_Not,       '!' },
  { Token_Type_And,       '&' },
  { Token_Type_Or,        '|' },
  { Token_Type_Percent,   '%' },

  { Token_Type_Pointer,   '^' },

  { Token_Type_Assign,    '=' },
  { Token_Type_Semicolon, ';' },
  { Token_Type_Colon,     ':' },
  { Token_Type_Comma,     ',' },
  { Token_Type_Less,      '>' },
  { Token_Type_More,      '<' },
  { Token_Type_Dot,       '.' },
  { Token_Type_Dollar,    '$' },

  { Token_Type_Open_Paren,    '(' },
  { Token_Type_Close_Paren,   ')' },
  { Token_Type_Open_Squirly,  '{' },
  { Token_Type_Close_Squirly, '}' },
  { Token_Type_Open_Square,   '[' },
  { Token_Type_Close_Square,  ']' },
};

#define ERROR_TYPES(X)            \
  X(Error_Type_Tokenizer)         \
  X(Error_Type_Syntax)            \
  X(Error_Type_Type)              \
  X(Error_Type_Internal)          \
  X(Error_Type_Linker)            \

X_ENUM(Error_Type, ERROR_TYPES);

#define errorc( type, location, msg)         error( (type), (location), (LIT(msg)))
#define errorf( type, location, format, ...) error( (type), (location), (fmt_tprintf(format, __VA_ARGS__)))
#define errorfc(type, location, format, ...) errorf((type), (location), LIT(format), __VA_ARGS__)

internal void error(Error_Type type, Token_Location location, String msg) {
  fmt_eprintflnc("\033[1;31m%S(%d:%d):\033[0m %S", location.file, location.line, location.column, msg);
}

internal b8 tokenize_file(String file, String path, Token_Vector *tokens) {
  b8 ok = true;
  isize start = 0, current = 0, line = 1, column = 1;

  while (current < file.len) {
    column += current - start;
    start   = current;

    rune c = utf8_rune_at(file, current, nil);

    if (c == '\n') {
      line    += 1;
      column   = 0;
      current += 1;
      continue;
    }

    if (c == '\t' || c == ' ' || c == '\r') {
      current += 1;
      continue;
    }

    Token token;

    if (c == '-') {
      rune c = utf8_rune_at(file, current + 1, nil);
      if (c == '>') {
        token.type = Token_Type_Arrow;
        current   += 2;
        goto append_token;
      }
    }

    if (c == '/') {
      if (utf8_rune_at(file, current + 1, nil) == '/') {
        loop {
          current += 1;
          c = utf8_rune_at(file, current, nil);
          if (c == '\n') {
            break;
          }
        }
        continue;
      }
    }

    if (c == '\'') {
      current += 1;
      c = utf8_rune_at(file, current, nil);
      if (c == '\\') {
        unimplemented();
      }
      current += 1;
      if (utf8_rune_at(file, current, nil) != '\'') {
        errorc(Error_Type_Tokenizer, ((Token_Location) {.file = path, .line = line, .column = column}), "Unterminated rune literal");
        ok = false;
      }
      token.literal.rune = c;
      token.type         = Token_Type_Rune;
      goto append_token;
    }

    slice_iter(c_array_to_slice(single_char_tokens), single_char_token, _i, {
       if (c == single_char_token->c) {
         token.type = single_char_token->type;
         current   += 1;
         goto append_token;
       }
    });

    if (c == '"') {
      current += 1;

      loop {
        c = utf8_rune_at(file, current, nil);
        current += 1;
        if (c == '"') {
          break;
        }
      }
      token.literal.string = slice_range(file, start + 1, current - 1);
      token.type           = Token_Type_String;
      goto append_token;
    }

    if (in_range(c, '0', '9' + 1)) {
      isize value = 0;
      loop {
        if (in_range(c, '0', '9' + 1)) {
          value *= 10;
          value += c - '0';
        }
        
        current += 1;
        c = utf8_rune_at(file, current, nil);

        if (!(c == '_' || in_range(c, '0', '9' + 1))) {
          break;
        }
      }

      if (c == '.') {
        f64 decimal = 0;
        f64 weight  = 1;
        loop {
          if (in_range(c, '0', '9' + 1)) {
            weight  /= 10;
            decimal += (c - '0') * weight;
          }
        
          current += 1;
          c = utf8_rune_at(file, current, nil);

          if (!(c == '_' || in_range(c, '0', '9' + 1))) {
            break;
          }
        }
        fmt_printflnc("value: %d, decimal: %f", value, decimal);
        token.literal.decimal = value + decimal;
        token.type            = Token_Type_Float;
        goto append_token;
      }

      token.literal.integer = value;
      token.type            = Token_Type_Int;
      goto append_token;
    }

    if (in_range(c, 'a', 'z' + 1) || in_range(c, 'A', 'Z' + 1) || c == '_') {
      while (in_range(c, 'a', 'z' + 1) || in_range(c, 'A', 'Z' + 1) || c == '_' || in_range(c, '0', '9' + 1)) {
        c = utf8_rune_at(file, current, nil);
        current += 1;
      }
      current -= 1;

      token.literal.string = slice_range(file, start, current);
      token.type           = Token_Type_Ident;
      
      slice_iter(c_array_to_slice(keyword_strings), keyword_string, _i, {
         if (string_equal(token.literal.string, keyword_string->string)) {
           token.type = keyword_string->type;
           break;
         }
      });

      goto append_token;
    }

    fmt_eprintflnc("Unexpected char: '%c', at line: %d, column: %d", c, line, column);
    current += 1;
    continue;

append_token:
    token.location = (Token_Location) {
      .line   = line,
      .file   = path,
      .column = column,
    };
    token.lexeme = slice_range(file, start, current);
    vector_append(tokens, token);
    continue;
  }
  
  return ok;
}

#define AST_NODE_TYPES(X) \
  X(Type_Pointer)         \
  X(Type_Array)           \
  X(Type_Named)           \
  X(Type_Enum)            \
  X(Type_Struct)          \
  X(Type_Union)           \
  X(Type_Function)        \
  X(Decl_Type)            \
  X(Decl_Function)        \
  X(Decl_Import)          \
  X(Field)                \

#define AST_ENUM_TO_STRING_CASES(v) case ANT_##v: return LIT("ANT_" #v);
#define AST_ENUM_TO_STRING(Enum, Variants)                                     \
  ENUM_TO_STRING_PROC_DECL(Enum, v) {                                          \
    switch (v) {                                                               \
      Variants(AST_ENUM_TO_STRING_CASES);                                      \
    default:                                                                   \
      return LIT(#Enum "_Invalid");                                            \
    }                                                                          \
  }

#define AST_ENUM_IS_VALID_CASES(v) case ANT_##v: return true;
#define AST_ENUM_IS_VALID(Enum, Variants)                                      \
  ENUM_IS_VALID_PROC_DECL(Enum, v) {                                           \
    switch (v) {                                                               \
      Variants(AST_ENUM_IS_VALID_CASES);                                       \
    default:                                                                   \
      return false;                                                            \
    }                                                                          \
  }

#define AST_ENUM_VARIANTS(v) ANT_##v,
#define AST_ENUM_VARIANTS_PRIVATE(v) internal const int NT_Ast_##v = ANT_##v;
#define AST_ENUM(Enum, Variants)                                               \
  typedef enum {                                                               \
    Variants(AST_ENUM_VARIANTS) Enum##__Max_Value,                             \
  } Enum;                                                                      \
                                                                               \
  Variants(AST_ENUM_VARIANTS_PRIVATE)                                          \
                                                                               \
  AST_ENUM_TO_STRING(Enum, Variants)                                           \
  AST_ENUM_IS_VALID(Enum, Variants)

AST_ENUM(Ast_Node_Type, AST_NODE_TYPES);

typedef struct _Ast_Node Ast_Node;

typedef struct _Ast_Node Ast_Expr;
typedef struct _Ast_Node Ast_Stmt;
typedef struct _Ast_Node Ast_Decl;

typedef struct {
  String    name;
  Ast_Expr *type;
} Ast_Field;

typedef Vector(Ast_Field *) Field_Vector;
typedef Vector(Ast_Stmt  *) Stmt_Vector;

typedef struct {
  Field_Vector args;
  Field_Vector returns;
  Stmt_Vector  *body;
} Ast_Decl_Function;

typedef struct {
  Field_Vector args;
  Field_Vector returns;
} Ast_Type_Function;

typedef struct {
  String    name;
  Ast_Expr *type;
} Ast_Decl_Type;

typedef struct {
  String    name;
  Ast_Expr *type;
  Ast_Expr *value;
  b8        constant;
} Ast_Decl_Variable;

typedef Vector(Ast_Stmt *) Ast_Stmt_Block;

typedef struct {
  Vector(Ast_Stmt *) body;
} Ast_Stmt_If;

typedef struct {
  String path;
} Ast_Decl_Import;

typedef struct {
  Ast_Expr *pointee;
} Ast_Type_Pointer;

typedef struct {
  Ast_Expr *elem;
  Ast_Expr *count;
} Ast_Type_Array;

typedef struct {
  String name;
} Ast_Type_Named;

typedef struct {
  Field_Vector fields;
} Ast_Type_Struct;

typedef struct {
  Field_Vector variants;
} Ast_Type_Union;

typedef struct {
  Vector(struct {
    String    name;
    Ast_Expr *value;
  }) values;
} Ast_Type_Enum;

typedef struct {
  String name;
} Ast_Expr_Ident;

typedef struct {
  String                    path;
  Vector(Ast_Decl_Import *) imports;
  Vector(Ast_Decl        *) decls;
} Ast_File;

struct _Ast_Node {
  Token_Location location;
  Ast_Node_Type  ast_type;

  union {
    Ast_Expr_Ident    expr_ident   [0];
    Ast_Expr_Unary    expr_unary   [0];
    Ast_Expr_Binary   expr_binary  [0];
    Ast_Expr_Ternary  expr_ternary [0];
    Ast_Expr_Call     expr_call    [0];
    Ast_Expr_Cast     expr_cast    [0];
    Ast_Expr_Index    expr_index   [0];
    
    Ast_Type_Pointer  type_pointer [0];
    Ast_Type_Array    type_array   [0];
    Ast_Type_Named    type_named   [0];
    Ast_Type_Struct   type_struct  [0];
    Ast_Type_Union    type_union   [0];
    Ast_Type_Enum     type_enum    [0];
    Ast_Type_Function type_function[0];

    Ast_Decl_Import   decl_import  [0];
    Ast_Decl_Function decl_function[0];
    Ast_Decl_Type     decl_type    [0];
    Ast_Decl_Variable decl_variable[0];
    Ast_Decl_Block    decl_block   [0];
    Ast_Decl_If       decl_if      [0];
    Ast_Decl_Loop     decl_loop    [0];
    Ast_Decl_Switch   decl_switch  [0];

    Ast_Field field[0];

    rawptr _union_accessor[0];
  };
};

#define ast_new(Type, _location, allocator)                                    \
  ({                                                                           \
    Ast_Node *ast_new_node = (Ast_Node *)(unwrap_err(mem_alloc_aligned(        \
        size_of(Ast_Node) + size_of(Type), 8, (allocator))));                  \
    ast_new_node->location = _location;                                        \
    ast_new_node->ast_type = NT_##Type;                                        \
    (Type *)&ast_new_node->_union_accessor;                                    \
  })

#define ast_base(node) (&((Ast_Node *)node)[-1])

typedef struct {
  Slice(Token) tokens;
  isize        current, errors;
  Ast_File     file;
} Parser;

internal Token parser_peek(Parser *parser) {
  if (parser->current >= parser->tokens.len) {
    return (Token) { .type = Token_Type_EOF };
  }
  Token t = IDX(parser->tokens, parser->current);
  return t;
}

internal Token parser_advance(Parser *parser) {
  if (parser->current >= parser->tokens.len) {
    return (Token) { .type = Token_Type_EOF };
  }
  Token t = IDX(parser->tokens, parser->current);
  parser->current += 1;
  return t;
}

internal void parser_skip(Parser *parser) {
  if (parser->current < parser->tokens.len) {
    parser->current += 1;
  }
}

internal b8 parser_peek_expect(Parser *parser, Token_Type type) {
  Token t = parser_peek(parser);
  return t.type == type;
}

internal b8 parser_expect(Parser *parser, Token_Type type, Token *t) {
  Token tok = parser_advance(parser);
  if (tok.type != type) {
    errorfc(Error_Type_Syntax, tok.location, "Expected %S but got '%S'", enum_to_string(Token_Type, type), tok.lexeme);
    return false;
  }
  if (t) {
    *t = tok;
  }
  return true;
}

internal void print_type(Ast_Expr *t) {
  switch (t->ast_type) {
  case ANT_Type_Function: {
    fmt_printc("fn(");
    vector_iter(t->type_function->args, arg, _i, {
      fmt_printfc("%S: ", (*arg)->name);
      print_type((*arg)->type);
      fmt_printfc(", ", (*arg)->name);
    });
    fmt_printc(") -> (");
    vector_iter(t->type_function->returns, return_, _i, {
      fmt_printfc("%S: ", (*return_)->name);
      print_type((*return_)->type);
      fmt_printfc(", ", (*return_)->name);
    });
    fmt_printc(")");
    break;
  }
  case ANT_Type_Pointer: {
    fmt_printc("^");
    print_type(t->type_pointer->pointee);
    break;
  }
  case ANT_Type_Array: {
    fmt_printc("[]");
    print_type(t->type_array->elem);
    break;
  }
  case ANT_Type_Enum: {
    fmt_printc("enum { ");
    vector_iter(t->type_enum->values, value, _i , {
      fmt_printfc("%S, ", value->name);
    });
    fmt_printc("}");
    break;
  }
  case ANT_Type_Struct: {
    fmt_printc("struct { ");
    vector_iter(t->type_struct->fields, field, _i, {
      fmt_printfc("%S: ", (*field)->name);
      print_type((*field)->type);
      fmt_printfc(", ", (*field)->name);
    });
    fmt_printc("}");
    break;
  }
  case ANT_Type_Union: {
    fmt_printc("union { ");
    vector_iter(t->type_union->variants, field, _i, {
      fmt_printfc("%S: ", (*field)->name);
      print_type((*field)->type);
      fmt_printfc(", ", (*field)->name);
    });
    fmt_printc("}");
    break;
  }
  case ANT_Type_Named: {
    fmt_print(t->type_named->name);
    break;
  }
  default:
    unreachable();
  }
}

internal Ast_Decl *parse_decl(Parser *parser, Allocator allocator) {
  return nil;
}

internal Ast_Expr *parse_expr(Parser *parser, Allocator allocator) {
  return nil;
}

internal b8 parse_fields(Parser *parser, Token_Type terminator, Field_Vector *fields, Allocator allocator);

internal Ast_Expr *parse_type(Parser *parser, Allocator allocator) {
  Token t = parser_advance(parser);
  switch (t.type) {
  case Token_Type_Open_Square: {
    Ast_Type_Array *array = ast_new(Ast_Type_Array, t.location, allocator);

    t = parser_advance(parser);
    if (t.type != Token_Type_Close_Square) {
      unimplemented();
      // array->count = parse_expr(parser, allocator);
      parser_expect(parser, Token_Type_Close_Square, nil);
    }
    array->elem = parse_type(parser, allocator);
    return ast_base(array);
  }
  case Token_Type_Struct: {
    Ast_Type_Struct *s = ast_new(Ast_Type_Struct, t.location, allocator);
    vector_init(&s->fields, 0, 8, allocator);

    if (!parser_expect(parser, Token_Type_Open_Squirly, nil)) {
      break;
    }
    parse_fields(parser, Token_Type_Close_Squirly, &s->fields, allocator);

    return ast_base(s);
  }
  case Token_Type_Union: {
    Ast_Type_Union *s = ast_new(Ast_Type_Union, t.location, allocator);
    vector_init(&s->variants, 0, 8, allocator);

    if (!parser_expect(parser, Token_Type_Open_Squirly, nil)) {
      break;
    }
    parse_fields(parser, Token_Type_Close_Squirly, &s->variants, allocator);

    return ast_base(s);
  }
  case Token_Type_Enum: {
    Ast_Type_Enum *e = ast_new(Ast_Type_Enum, t.location, allocator);
    vector_init(&e->values, 0, 8, allocator);

    if (!parser_expect(parser, Token_Type_Open_Squirly, nil)) {
      break;
    }

    for (Token t = parser_peek(parser); t.type != Token_Type_Close_Squirly; t = parser_peek(parser)) {
      Token ident;
      if (!parser_expect(parser, Token_Type_Ident, &ident)) {
        return false;
      }
      type_of(e->values.data[0]) v = {0};
      v.name = ident.literal.string;

      if (parser_peek(parser).type == Token_Type_Assign) {
        parser_advance(parser);

        v.value = parse_expr(parser, allocator);
      }
      
      vector_append(&e->values, v);

      if (!parser_peek_expect(parser, Token_Type_Comma)) {
        break;
      }
      parser_advance(parser);
    }
    if (!parser_expect(parser, Token_Type_Close_Squirly, nil)) {
      break;
    }

    return ast_base(e);
  }
  case Token_Type_Pointer: {
    Ast_Type_Pointer *pointer = ast_new(Ast_Type_Pointer, t.location, allocator);
    pointer->pointee  = parse_type(parser, allocator);
    return ast_base(pointer);
  }
  case Token_Type_Ident: {
    Ast_Type_Named *type = ast_new(Ast_Type_Named, t.location, allocator);
    type->name = t.literal.string;
    return ast_base(type);
  }
  case Token_Type_Fn: {
    if (!parser_expect(parser, Token_Type_Open_Paren, nil)) {
      return false;
    }

    Field_Vector args = vector_make(Field_Vector, 0, 8, allocator);
    parse_fields(parser, Token_Type_Close_Paren, &args, allocator);

    Field_Vector returns = {0};
    Token t2 = parser_peek(parser);
    if (t2.type == Token_Type_Arrow) {
      parser_advance(parser);
      parser_expect(parser, Token_Type_Open_Paren, nil);
      returns = vector_make(Field_Vector, 0, 8, allocator);
      parse_fields(parser, Token_Type_Close_Paren, &returns, allocator);
    }

    Ast_Type_Function *fn = ast_new(Ast_Type_Function, t.location, allocator);
    fn->args     = args;
    fn->returns  = returns;
    return ast_base(fn);
  }
  default:
    errorc(Error_Type_Syntax, t.location, "Expect type");
    return nil;
  }

  return nil;
}

internal b8 parse_fields(Parser *parser, Token_Type terminator, Field_Vector *fields, Allocator allocator) {
  for (Token t = parser_peek(parser); t.type != terminator; t = parser_peek(parser)) {
    Token ident;
    if (!parser_expect(parser, Token_Type_Ident, &ident)) {
      return false;
    }
    if (!parser_expect(parser, Token_Type_Colon, nil)) {
      return false;
    }
    Ast_Expr *type = parse_type(parser, allocator);

    Ast_Field *field = ast_new(Ast_Field, ident.location, allocator);
    field->name = ident.literal.string;
    field->type = type;
    vector_append(fields, field);

    if (!parser_peek_expect(parser, Token_Type_Comma)) {
      break;
    }
    parser_advance(parser);
  }
  parser_expect(parser, terminator, nil);
  return true;
}

internal Ast_Decl_Function *parse_function(Parser *parser, Allocator allocator) {
  Token t;
  parser_expect(parser, Token_Type_Fn, &t);

  Token ident;
  if (!parser_expect(parser, Token_Type_Ident, &ident)) {
    return false;
  }

  if (!parser_expect(parser, Token_Type_Open_Paren, nil)) {
    return false;
  }

  Field_Vector args = vector_make(Field_Vector, 0, 8, allocator);
  parse_fields(parser, Token_Type_Close_Paren, &args, allocator);

  Field_Vector returns = {0};
  Token t2 = parser_peek(parser);
  if (t2.type == Token_Type_Arrow) {
    parser_advance(parser);
    parser_expect(parser, Token_Type_Open_Paren, nil);
    returns = vector_make(Field_Vector, 0, 8, allocator);
    parse_fields(parser, Token_Type_Close_Paren, &returns, allocator);
  }
  parser_expect(parser, Token_Type_Open_Squirly,  nil);
  parser_expect(parser, Token_Type_Close_Squirly, nil);

  Ast_Decl_Function *fn = ast_new(Ast_Decl_Function, t.location, allocator);
  fn->args     = args;
  fn->returns  = returns;
  return fn;
}

internal void parse_file(Parser *parser, Allocator allocator) {
  while (parser->current < parser->tokens.len) {
    Token t = parser_peek(parser);

    switch (t.type) {
    case Token_Type_Fn: {
      Ast_Decl_Function *func_decl = parse_function(parser, allocator);
      vector_append(&parser->file.decls, ast_base(func_decl));

      vector_iter(func_decl->args, arg, i, {
        fmt_printfc("%S: ", (*arg)->name);
        print_type((*arg)->type);
        fmt_printlnc("");
      });
      vector_iter(func_decl->returns, return_value, i, {
        fmt_printflnc("return_value: '%S'", (*return_value)->name);
      });
      break;
    }
    case Token_Type_Def:
      parser_skip(parser);
      Token ident;
      if (!parser_expect(parser, Token_Type_Ident, &ident)) {
        break;
      }
      Ast_Expr *type = parse_type(parser, allocator);
      Ast_Decl_Type *type_decl = ast_new(Ast_Decl_Type, t.location, allocator);
      type_decl->type = type;
      type_decl->name = ident.literal.string;
      vector_append(&parser->file.decls, ast_base(type_decl));
      parser_expect(parser, Token_Type_Semicolon, &ident);
      break;
    case Token_Type_Import:
      parser_skip(parser);
      Token t2;
      if (!parser_expect(parser, Token_Type_String, &t2)) {
        break;
      }
      Ast_Decl_Import *import = ast_new(Ast_Decl_Import, t.location, allocator);
      import->path = t2.literal.string;
      vector_append(&parser->file.imports, import);
      if (!parser_expect(parser, Token_Type_Semicolon, &t2)) {
        break;
      }
      break;

    case Token_Type_EOF:
      return;

    default:
      parser_skip(parser);
      errorfc(Error_Type_Syntax, t.location, "Unexpected token at file scope: '%S'", t.lexeme);
    }
  }
}

i32 main() {
  Tracking_Allocator track = {0};
  context.allocator = tracking_allocator_init(&track, context.allocator);
  context.logger    = create_file_logger(1);

  if (os_args.len < 2) {
    fmt_eprintflnc("Usage: %S <file> <flags>", os_args.data[0]);
    os_exit(1);
  }

  slice_iter(slice_start(os_args, 1), path, _i, {
    Byte_Slice file_data = or_do_err(read_entire_file_path(*path, context.temp_allocator), err, {
      fmt_eprintflnc("Failed to read file '%S': '%S'", *path, enum_to_string(OS_Error, err));
      continue;
    });
    Token_Vector tokens = vector_make(Token_Vector, 0, 8, context.temp_allocator);
    if (!tokenize_file(bytes_to_string(file_data), *path, &tokens)) {
      fmt_eprintflnc("Failed to tokenize file '%S'", *path);
    }

    Parser parser    = {0};
    parser.tokens    = vector_to_slice(type_of(parser.tokens), tokens);
    parser.file.path = *path;
    vector_init(&parser.file.decls, 0, 8, context.temp_allocator);

    parse_file(&parser, context.temp_allocator);

    slice_iter(parser.file.imports, import, _i, {
      fmt_printflnc("Import: '%S'", (*import)->path);
    });

    slice_iter(parser.file.decls, td, _i, {
      if ((*td)->ast_type == NT_Ast_Decl_Type) {
        fmt_printfc("decl: '%S = ", (*td)->decl_type->name);
        print_type((*td)->decl_type->type);
        fmt_printlnc("'");
      }
    });
  });

  tracking_allocator_fmt_results_w(&stdout, &track);
  tracking_allocator_destroy(track);
  return 0;
}
