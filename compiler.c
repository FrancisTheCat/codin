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
  { Token_Type_Enum,     LIT("enum"),     },
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
  X(ANT_Ident_Expr)       \
  X(ANT_Paren_Expr)       \
  X(ANT_Call_Expr)        \
  X(ANT_Unary_Expr)       \
  X(ANT_Binary_Expr)      \
  X(ANT_Ternary_Expr)     \
  X(ANT_Index_Stmt)       \
                          \
  X(ANT_Pointer_Type)     \
  X(ANT_Array_Type)       \
  X(ANT_Dynamic_Array_Type)\
  X(ANT_Struct_Type)      \
  X(ANT_Enum_Type)        \
  X(ANT_Union_Type)       \
  X(ANT_Named_Type)       \
                          \
  X(ANT_Iterator_Stmt)    \
  X(ANT_Block_Stmt)       \
  X(ANT_Conditional_Stmt) \
  X(ANT_Loop_Stmt)        \
  X(ANT_Assign_Stmt)      \
  X(ANT_Expr_Stmt)        \
  X(ANT_Defer_Stmt)       \
  X(ANT_Return_Stmt)      \
  X(ANT_Branch_Stmt)      \
                          \
  X(ANT_Field)            \
                          \
  X(ANT_Operator_Decl)    \
  X(ANT_Function_Decl)    \
  X(ANT_Struct_Decl)      \
  X(ANT_Type_Decl)        \
  X(ANT_Import_Decl)      \

X_ENUM(Ast_Node_Type, AST_NODE_TYPES);

#define AST_BASE           \
  Token_Location location; \
  Ast_Node_Type  ast_type

typedef struct {
  AST_BASE;
} Ast_Node;

#define AST_EXPR_BASE AST_BASE

typedef struct {
  AST_BASE;
} Ast_Expr;

typedef struct {
  AST_BASE;

  String    name;
  Ast_Expr *type;
} Ast_Field;

typedef struct {
  AST_BASE;

  String    name;
  Ast_Expr *type;
} Ast_Decl_Type;

typedef struct {
  AST_BASE;
} Ast_Stmt;

typedef Vector(Ast_Field) Fields_Vector;
typedef Vector(Ast_Stmt)  Stmt_Vector;

typedef struct {
  AST_BASE;

  Fields_Vector args;
  Fields_Vector returns;
  Stmt_Vector  *body;
} Ast_Decl_Function;

typedef Ast_Node Ast_Decl;

typedef struct {
  AST_BASE;

  String path;
} Ast_Import;

typedef struct {
  AST_EXPR_BASE;
  Ast_Expr *pointee;
} Ast_Type_Pointer;

typedef struct {
  AST_EXPR_BASE;

  Ast_Expr *elem;
  Ast_Expr *count;
} Ast_Type_Array;

typedef struct {
  AST_EXPR_BASE;
  String name;
} Ast_Type_Named;

typedef struct {
  String             path;
  Vector(Ast_Import) imports;
  Vector(Ast_Decl *) decls;
} Ast_File;

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

internal void print_type(Ast_Expr *type) {
  fmt_printflnc("print_type: %x", type);
  switch (type->ast_type) {
  case ANT_Pointer_Type: {
    fmt_printc("^");
    print_type(((Ast_Type_Pointer *)type)->pointee);
    break;
  }
  case ANT_Array_Type: {
    fmt_printc("[]");
    print_type(((Ast_Type_Array *)type)->elem);
    break;
  }
  case ANT_Dynamic_Array_Type: {
    fmt_printc("[dynamic]");
    print_type(((Ast_Type_Array *)type)->elem);
    break;
  }
  case ANT_Struct_Type: {
    
    break;
  }
  case ANT_Enum_Type: {
    
    break;
  }
  case ANT_Union_Type: {
    
    break;
  }
  case ANT_Named_Type: {
    fmt_print(((Ast_Type_Named *)type)->name);
    break;
  }
  default:
    unreachable();
  }
}

internal Ast_Expr *parse_type(Parser *parser, Allocator allocator) {
  Token t = parser_advance(parser);
  switch (t.type) {
  case Token_Type_Open_Square: {
    Ast_Type_Array *array = mem_tnew(Ast_Type_Array, allocator);
    array->ast_type = ANT_Array_Type;
    array->location = t.location;

    t = parser_advance(parser);
    if (t.type != Token_Type_Close_Square) {
      unimplemented();
      // array->count = parse_expr(parser, allocator);
      parser_expect(parser, Token_Type_Close_Square, nil);
    }
    array->elem = parse_type(parser, allocator);
    return (Ast_Expr *)array;
    break;
  }
  case Token_Type_Struct:
    unimplemented();
    break;
  case Token_Type_Union:
    unimplemented();
    break;
  case Token_Type_Enum:
    unimplemented();
    break;
  case Token_Type_Pointer: {
    Ast_Type_Pointer *pointer = mem_tnew(Ast_Type_Pointer, allocator);
    pointer->ast_type = ANT_Pointer_Type;
    pointer->location = t.location;
    pointer->pointee  = parse_type(parser, allocator);
    return (Ast_Expr *)pointer;
  }
  case Token_Type_Ident: {
    Ast_Type_Named *type = mem_tnew(Ast_Type_Named, allocator);
    type->location = t.location;
    type->ast_type = ANT_Named_Type;
    type->name     = t.literal.string;
    return (Ast_Expr *)type;
  }
  default:
    errorc(Error_Type_Syntax, t.location, "Expect type");
    return nil;
  }

  return nil;
}

internal b8 parse_fields(Parser *parser, Token_Type terminator, Fields_Vector *fields, Allocator allocator) {
  for (Token t = parser_peek(parser); t.type != terminator; t = parser_peek(parser)) {
    Token ident;
    if (!parser_expect(parser, Token_Type_Ident, &ident)) {
      return false;
    }
    if (!parser_expect(parser, Token_Type_Colon, &ident)) {
      return false;
    }
    Ast_Expr *type = parse_type(parser, allocator);

    Ast_Field field = {
      .ast_type = ANT_Field,
      .location = ident.location,
      .name     = ident.literal.string,
      .type     = type,
    };

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

  Fields_Vector args = vector_make(Fields_Vector, 0, 8, allocator);
  parse_fields(parser, Token_Type_Close_Paren, &args, allocator);

  Fields_Vector returns;
  Token t2 = parser_peek(parser);
  if (t2.type == Token_Type_Arrow) {
    parser_advance(parser);
    parser_expect(parser, Token_Type_Open_Paren, nil);
    returns = vector_make(Fields_Vector, 0, 8, allocator);
    parse_fields(parser, Token_Type_Close_Paren, &returns, allocator);
  }
  parser_expect(parser, Token_Type_Open_Squirly,  nil);
  parser_expect(parser, Token_Type_Close_Squirly, nil);

  Ast_Decl_Function *fn = mem_tnew(Ast_Decl_Function, allocator);
  fn->args     = args;
  fn->returns  = returns;
  fn->ast_type = ANT_Function_Decl;
  fn->location = t.location;

  return fn;
}

internal void parse_file(Parser *parser, Allocator allocator) {
  while (parser->current < parser->tokens.len) {
    Token t = parser_peek(parser);

    switch (t.type) {
    case Token_Type_Fn: {
      Ast_Decl_Function *func_decl = parse_function(parser, allocator);
      vector_append(&parser->file.decls, (Ast_Decl *)func_decl);

      vector_iter(func_decl->args, arg, i, {
        fmt_printflnc("arg: '%S'", arg->name);
      });
      vector_iter(func_decl->returns, return_value, i, {
        fmt_printflnc("return_value: '%S'", return_value->name);
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
      Ast_Decl_Type *type_decl = mem_tnew(Ast_Decl_Type, allocator);
      type_decl->ast_type = ANT_Type_Decl;
      type_decl->location = t.location;
      type_decl->type     = type;
      type_decl->name     = ident.literal.string;
      vector_append(&parser->file.decls, (Ast_Decl *)type_decl);
      parser_expect(parser, Token_Type_Semicolon, &ident);

      fmt_printflnc("type: '%x'", type_decl);
      print_type(type);
      fmt_printflnc("\ndecl: '%x'", (Ast_Expr *)IDX(parser->file.decls, parser->file.decls.len - 1));
      print_type(((Ast_Decl_Type *)parser->file.decls.data[parser->file.decls.len - 1])->type);
      break;
    case Token_Type_Import:
      parser_skip(parser);
      Token t2;
      if (!parser_expect(parser, Token_Type_String, &t2)) {
        break;
      }
      Ast_Import import = { .location = t.location, .ast_type = ANT_Import_Decl, .path = t2.literal.string };
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
  Tracking_Allocator track;
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
    Token_Vector tokens = vector_make(Token_Vector, 0, 8, context.allocator);
    if (!tokenize_file(bytes_to_string(file_data), *path, &tokens)) {
      fmt_eprintflnc("Failed to tokenize file '%S'", *path);
    }

    Parser parser    = {0};
    parser.tokens    = vector_to_slice(type_of(parser.tokens), tokens);
    parser.file.path = *path;
    vector_init(&parser.file.decls, 0, 8, context.allocator);

    parse_file(&parser, context.temp_allocator);

    slice_iter(parser.file.imports, import, _i, {
      fmt_printflnc("Import: '%S'", import->path);
    });

    slice_iter(parser.file.decls, decl, _i, {
      if ((*decl)->ast_type == ANT_Type_Decl) {
        fmt_printfc("decl: '%S = ", ((Ast_Decl_Type *)decl)->name);
        print_type(((Ast_Decl_Type *)decl)->type);
        fmt_printlnc("'");
      }
    });

    // slice_iter(tokens, token, i, {
    //   switch (token->type) {
    //     case Token_Type_String: {
    //       fmt_printflnc(
    //         "%S('%S') at %d:%d",
    //         enum_to_string(Token_Type, token->type),
    //         token->literal.string,
    //         token->location.line,
    //         token->location.column
    //       );
    //       break;
    //     }
    //     case Token_Type_Float: {
    //       fmt_printflnc(
    //         "%S('%f') at %d:%d",
    //         enum_to_string(Token_Type, token->type),
    //         (f32)token->literal.decimal,
    //         token->location.line,
    //         token->location.column
    //       );
    //       break;
    //     }
    //     case Token_Type_Int: {
    //       fmt_printflnc(
    //         "%S('%d') at %d:%d",
    //         enum_to_string(Token_Type, token->type),
    //         token->literal.integer,
    //         token->location.line,
    //         token->location.column
    //       );
    //       break;
    //     }
    //     case Token_Type_Ident: {
    //       fmt_printflnc(
    //         "%S('%S') at %d:%d",
    //         enum_to_string(Token_Type, token->type),
    //         token->literal.string,
    //         token->location.line,
    //         token->location.column
    //       );
    //       break;
    //     }
    //     default: {
    //       fmt_printflnc(
    //         "%S at %d:%d",
    //         enum_to_string(Token_Type, token->type),
    //         token->location.line,
    //         token->location.column
    //       );
    //     }
    //   }
    // });
  });

  tracking_allocator_fmt_results_w(&stdout, &track);
  tracking_allocator_destroy(track);
  return 0;
}
