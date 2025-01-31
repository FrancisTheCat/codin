#include "codin.h"

#define TOKENS(X)                                                              \
  X(Token_Type_String)                                                         \
  X(Token_Type_Int)                                                            \
  X(Token_Type_Float)                                                          \
  X(Token_Type_Rune)                                                           \
  X(Token_Type_Ident)                                                          \
                                                                               \
  X(Token_Type_Assign)                                                         \
  X(Token_Type_Assign_Op)                                                      \
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
  X(Token_Type_Bit_And)                                                        \
  X(Token_Type_Bit_Or)                                                         \
  X(Token_Type_Less)                                                           \
  X(Token_Type_More)                                                           \
  X(Token_Type_Equal)                                                          \
  X(Token_Type_Less_Equal)                                                     \
  X(Token_Type_More_Equal)                                                     \
  X(Token_Type_Modulo)                                                         \
                                                                               \
  X(Token_Type_Open_Paren)                                                     \
  X(Token_Type_Close_Paren)                                                    \
  X(Token_Type_Open_Squirly)                                                   \
  X(Token_Type_Close_Squirly)                                                  \
  X(Token_Type_Open_Square)                                                    \
  X(Token_Type_Close_Square)                                                   \
                                                                               \
  X(Token_Type_Cast)                                                           \
  X(Token_Type_Transmute)                                                      \
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
    union {
      String string;
      isize  integer;
      rune   rune;
      f64    decimal;
    } literal;
    Token_Type assign_op;
  };
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

  { Token_Type_Cast,      LIT("cast"),      },
  { Token_Type_Transmute, LIT("transmute"), },
};

internal rune single_char_tokens[] = {
  ['+'] = Token_Type_Plus            ,
  ['-'] = Token_Type_Minus           ,
  ['*'] = Token_Type_Multiply        ,
  ['/'] = Token_Type_Divide          ,
  ['!'] = Token_Type_Not             ,
  ['&'] = Token_Type_And             ,
  ['|'] = Token_Type_Or              ,
  ['%'] = Token_Type_Modulo          ,

  ['^'] = Token_Type_Pointer         ,

  ['='] = Token_Type_Assign          ,
  [';'] = Token_Type_Semicolon       ,
  [':'] = Token_Type_Colon           ,
  [','] = Token_Type_Comma           ,
  ['>'] = Token_Type_Less            ,
  ['<'] = Token_Type_More            ,
  ['.'] = Token_Type_Dot             ,
  ['$'] = Token_Type_Dollar          ,

  ['('] = Token_Type_Open_Paren      ,
  [')'] = Token_Type_Close_Paren     ,
  ['{'] = Token_Type_Open_Squirly    ,
  ['}'] = Token_Type_Close_Squirly   ,
  ['['] = Token_Type_Open_Square     ,
  [']'] = Token_Type_Close_Square    ,
};

internal String binary_op_strings[] = {
  [Token_Type_Plus      ] = LIT("+" ),
  [Token_Type_Minus     ] = LIT("-" ),
  [Token_Type_Multiply  ] = LIT("*" ),
  [Token_Type_Divide    ] = LIT("/" ),
  [Token_Type_And       ] = LIT("&&"),
  [Token_Type_Or        ] = LIT("||"),
  [Token_Type_Bit_And   ] = LIT("&" ),
  [Token_Type_Bit_Or    ] = LIT("|" ),
  [Token_Type_Less      ] = LIT("<" ),
  [Token_Type_More      ] = LIT(">" ),
  [Token_Type_Equal     ] = LIT("=="),
  [Token_Type_Less_Equal] = LIT("<="),
  [Token_Type_More_Equal] = LIT(">="),
  [Token_Type_Modulo    ] = LIT("%" ),
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

    Token token;

    switch (c) {
    case '\n': {
      line    += 1;
      column   = 0;
      current += 1;
      continue;
    }
    case '\t':
    case ' ':
    case '\r': {
      current += 1;
      continue;
    }
    case '-': {
      rune c = utf8_rune_at(file, current + 1, nil);
      if (c == '>') {
        token.type = Token_Type_Arrow;
        current   += 2;
      } else if (c == '=') {
        token.type = Token_Type_Assign_Op;
        current   += 2;
      } else {
        token.type = Token_Type_Minus;
        current   += 1;
      }
      goto append_token;
    }
    case '/': {
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
    case '+':
    case '*':
    case '=':
    case '>':
    case '<':
    case '&':
    case '|': {
        rune c2 = utf8_rune_at(file, current + 1, nil);
        if (c2 == '=') {
          token.type      = Token_Type_Assign_Op;
          token.assign_op = single_char_tokens[c];
          current        += 2;
          switch (c) {
          case '=': {
              token.type = Token_Type_Equal;
              break;
            }
          case '>': {
              token.type = Token_Type_More_Equal;
              break;
            }
          case '<': {
              token.type = Token_Type_Less_Equal;
              break;
            }
          }
        } else {
          token.type      = single_char_tokens[c];
          current        += 1;
        }
        goto append_token;
      }

    case '^':
    case ';':
    case ':':
    case ',':
    case '.':
    case '$':
    case '(':
    case ')':
    case '{':
    case '}':
    case '[':
    case ']': {
        current += 1;
        token.type = single_char_tokens[c];
        goto append_token;
      }

    case '\'': {
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

    case '"': {
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

    case '0' ... '9': {
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
          token.literal.decimal = value + decimal;
          token.type            = Token_Type_Float;
          goto append_token;
        }

        token.literal.integer = value;
        token.type            = Token_Type_Int;
        goto append_token;
      }

    case 'a' ... 'z':
    case 'A' ... 'Z':
    case '_': {
        while (in_range(c, 'a', 'z' + 1) || in_range(c, 'A', 'Z' + 1) || c == '_' || in_range(c, '0', '9' + 1)) {
          c = utf8_rune_at(file, current, nil);
          current += 1;
        }
        current -= 1;

        token.literal.string = slice_range(file, start, current);
        token.type           = Token_Type_Ident;
      
        slice_iter_v(c_array_to_slice(keyword_strings), keyword_string, _i, {
           if (string_equal(token.literal.string, keyword_string.string)) {
             token.type = keyword_string.type;
             break;
           }
        });

        goto append_token;
      }
    }

    fmt_eprintflnc("Unexpected character: '%c', at line: %d, column: %d", c, line, column);
    ok = false;
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

#define AST_NODE_TYPES(X)                                                      \
  X(Expr_Ident)                                                                \
  X(Expr_Literal)                                                              \
  X(Expr_Unary)                                                                \
  X(Expr_Binary)                                                               \
  X(Expr_Ternary)                                                              \
  X(Expr_Call)                                                                 \
  X(Expr_Selector)                                                             \
  X(Expr_Deref)                                                                \
  X(Expr_Cast)                                                                 \
  X(Expr_Index)                                                                \
  X(Expr_Address)                                                              \
                                                                               \
  X(Type_Pointer)                                                              \
  X(Type_Array)                                                                \
  X(Type_Named)                                                                \
  X(Type_Struct)                                                               \
  X(Type_Union)                                                                \
  X(Type_Enum)                                                                 \
  X(Type_Function)                                                             \
                                                                               \
  X(Decl_Import)                                                               \
  X(Decl_Function)                                                             \
  X(Decl_Type)                                                                 \
  X(Decl_Variable)                                                             \
                                                                               \
  X(Stmt_Defer)                                                                \
  X(Stmt_Return)                                                               \
  X(Stmt_Break)                                                                \
  X(Stmt_Continue)                                                             \
  X(Stmt_Block)                                                                \
  X(Stmt_If)                                                                   \
  X(Stmt_Loop)                                                                 \
  X(Stmt_Iterator)                                                             \
  X(Stmt_Switch)                                                               \
  X(Stmt_Assign)                                                               \
  X(Field)

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
  Ast_Expr *value;
} Ast_Field;

typedef Vector(Ast_Field *) Field_Vector;
typedef Vector(Ast_Stmt  *) Ast_Stmt_Block;

typedef struct {
  String         name;
  Field_Vector   args;
  Field_Vector   returns;
  Ast_Stmt_Block body;
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

typedef struct {
  Ast_Expr      *cond;
  Ast_Stmt_Block body;
} Ast_Stmt_If;

typedef struct {
  Ast_Stmt *rhs;
} Ast_Stmt_Defer;

typedef struct {
  Vector(Ast_Expr *) values;
} Ast_Stmt_Return;

typedef struct {
  String label;
} Ast_Stmt_Break;

typedef struct {
  String label;
} Ast_Stmt_Continue;

typedef struct {
  Ast_Decl      *pre;
  Ast_Expr      *cond;
  Ast_Stmt      *post;
  Ast_Stmt_Block body;
} Ast_Stmt_For;

typedef struct {
  Ast_Expr      *begin;
  Ast_Expr      *end;
  Ast_Decl      *decl;
  Ast_Stmt_Block body;
} Ast_Stmt_Loop;

typedef struct {
  Ast_Expr      *lhs;
  Field_Vector   decls;
  Ast_Stmt_Block body;
} Ast_Stmt_Iterator;

typedef struct {
  Vector(struct {
    Ast_Expr      *case_;
    Ast_Stmt_Block block;
  }) body;
} Ast_Stmt_Switch;

typedef struct {
  Ast_Expr *lhs;
  Ast_Expr *rhs;
} Ast_Stmt_Assign;

typedef struct {
  Ast_Expr *expr;
} Ast_Stmt_Expr;

typedef struct {
  Ast_Decl *decl;
} Ast_Stmt_Decl;

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
  Token_Type type;
  union {
    String string;
    isize  integer;
    rune   rune;
    f64    decimal;
  } value;
} Ast_Expr_Literal;

typedef struct {
  Token_Type op;
  Ast_Expr  *rhs;
} Ast_Expr_Unary;

typedef struct {
  Token_Type op;
  Ast_Expr  *lhs;
  Ast_Expr  *rhs;
} Ast_Expr_Binary;

typedef struct {
  Token_Type op;
  Ast_Expr  *cond;
  Ast_Expr  *lhs;
  Ast_Expr  *rhs;
} Ast_Expr_Ternary;

typedef struct {
  Ast_Expr        *  lhs;
  Vector(Ast_Expr *) args;
} Ast_Expr_Call;

typedef struct {
  Ast_Expr *lhs;
  String    selector;
} Ast_Expr_Selector;

typedef struct {
  Ast_Expr *lhs;
} Ast_Expr_Deref;

typedef struct {
  Ast_Expr *type;
  Ast_Expr *rhs;
  b8        bitwise;
} Ast_Expr_Cast;

typedef struct {
  Ast_Expr *base;
  Ast_Expr *index;
} Ast_Expr_Index;

typedef struct {
  String             path, package;
  Vector(Ast_Decl *) decls;
} Ast_File;

struct _Ast_Node {
  Token_Location location;
  Ast_Node_Type  ast_type;

  union {
    Ast_Expr_Ident    expr_ident   [0];
    Ast_Expr_Literal  expr_literal [0];
    Ast_Expr_Unary    expr_unary   [0];
    Ast_Expr_Binary   expr_binary  [0];
    Ast_Expr_Ternary  expr_ternary [0];
    Ast_Expr_Call     expr_call    [0];
    Ast_Expr_Selector expr_selector[0];
    Ast_Expr_Deref    expr_deref   [0];
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

    Ast_Stmt_Defer    stmt_defer   [0];
    Ast_Stmt_Return   stmt_return  [0];
    Ast_Stmt_Block    stmt_block   [0];
    Ast_Stmt_If       stmt_if      [0];
    Ast_Stmt_Loop     stmt_loop    [0];
    Ast_Stmt_Iterator stmt_iterator[0];
    Ast_Stmt_Switch   stmt_switch  [0];
    Ast_Stmt_Assign   stmt_assign  [0];
    Ast_Stmt_Expr     stmt_expr    [0];
    Ast_Stmt_Decl     stmt_decl    [0];

    Ast_Field         field[0];

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
  Token t = *IDX(parser->tokens, parser->current);
  return t;
}

internal Token parser_peek_n(Parser *parser, int n) {
  if (parser->current + n >= parser->tokens.len) {
    return (Token) { .type = Token_Type_EOF };
  }
  Token t = *IDX(parser->tokens, parser->current + n);
  return t;
}

internal Token parser_advance(Parser *parser) {
  if (parser->current >= parser->tokens.len) {
    return (Token) { .type = Token_Type_EOF };
  }
  Token t = *IDX(parser->tokens, parser->current);
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
    errorfc(Error_Type_Syntax, tok.location, "Expected %S but got '%S' (type: %S)", enum_to_string(Token_Type, type), tok.lexeme, enum_to_string(Token_Type, tok.type));
    return false;
  }
  if (t) {
    *t = tok;
  }
  return true;
}

internal Ast_Decl *parse_decl(Parser *parser, Allocator allocator);
internal Ast_Stmt *parse_stmt(Parser *parser, Allocator allocator);
internal Ast_Expr *parse_type(Parser *parser, Allocator allocator);
internal Ast_Expr *parse_expr(Parser *parser, Allocator allocator);

internal void print_expr(Ast_Expr *e);
internal void print_type(Ast_Expr *e);
internal void print_stmt(Ast_Stmt *s);
internal void print_decl(Ast_Decl *d);

internal Ast_Expr *parse_atom_expr(Parser *parser, Allocator allocator) {
  Token t = parser_advance(parser);

  switch (t.type) {
  case Token_Type_Ident: {
      Ast_Expr_Ident *ident = ast_new(Ast_Expr_Ident, t.location, allocator);
      ident->name = t.literal.string;
      return ast_base(ident);
    }
  case Token_Type_String: {
      Ast_Expr_Literal *literal = ast_new(Ast_Expr_Literal, t.location, allocator);
      literal->value.string = t.literal.string;
      literal->type         = t.type;
      return ast_base(literal);
    }
  case Token_Type_Int: {
      Ast_Expr_Literal *literal = ast_new(Ast_Expr_Literal, t.location, allocator);
      literal->value.integer = t.literal.integer;
      literal->type          = t.type;
      return ast_base(literal);
    }
  case Token_Type_Float: {
      Ast_Expr_Literal *literal = ast_new(Ast_Expr_Literal, t.location, allocator);
      literal->value.decimal = t.literal.decimal;
      literal->type          = t.type;
      return ast_base(literal);
    }
  case Token_Type_Open_Paren: {
      Ast_Expr *expr = parse_expr(parser, allocator);
      parser_expect(parser, Token_Type_Close_Paren, nil);
      return expr;
    }
  case Token_Type_Transmute:
  case Token_Type_Cast: {
      Ast_Expr_Cast *cast = ast_new(Ast_Expr_Cast, t.location, allocator);
      cast->bitwise = t.type == Token_Type_Transmute;

      parser_expect(parser, Token_Type_Open_Paren, nil);
      cast->type = parse_type(parser, allocator);
      parser_expect(parser, Token_Type_Close_Paren, nil);
      cast->rhs = parse_expr(parser, allocator);

      return ast_base(cast);
    }
  case Token_Type_And:
  case Token_Type_Plus:
  case Token_Type_Minus: {
      Ast_Expr_Unary *unary = ast_new(Ast_Expr_Unary, t.location, allocator);
      unary->op  = t.type;
      unary->rhs = parse_expr(parser, allocator);
      return ast_base(unary);
    }
  default:
    errorfc(Error_Type_Syntax, t.location, "Failed to parse atomic expression: unexpected token: '%S'", t.lexeme);
    return nil;
  }

  unreachable();
}

internal isize op_binding_powers[enum_len(Token_Type)] = {
  [Token_Type_Modulo  ] = 7,
  [Token_Type_Multiply] = 6,
  [Token_Type_Divide  ] = 6,
  [Token_Type_Minus   ] = 5,
  [Token_Type_Plus    ] = 5,

  [Token_Type_And     ] = 4,
  [Token_Type_Or      ] = 3,
  [Token_Type_More    ] = 2,
  [Token_Type_Less    ] = 2,
  [Token_Type_Equal   ] = 1,
};

internal Ast_Expr *_parse_expr(Parser *parser, isize min_bp, Allocator allocator) {
  Token_Location start_location = parser_peek(parser).location;
  Ast_Expr *lhs = parse_atom_expr(parser, allocator);

  loop {
    Token t = parser_peek(parser);
    switch (t.type) {
    case Token_Type_More_Equal:
    case Token_Type_Less_Equal:
    case Token_Type_Multiply:
    case Token_Type_Modulo:
    case Token_Type_Divide:
    case Token_Type_Equal:
    case Token_Type_Minus:
    case Token_Type_More:
    case Token_Type_Less:
    case Token_Type_Plus:
    case Token_Type_And:
    case Token_Type_Or: {
        isize bp = op_binding_powers[t.type];
        if (bp < min_bp) {
          return lhs;
        }
        parser_advance(parser);
        Ast_Expr *rhs = _parse_expr(parser, bp, allocator);
        Ast_Expr_Binary *b = ast_new(Ast_Expr_Binary, start_location, allocator);
        b->op  = t.type;
        b->lhs = lhs;
        b->rhs = rhs;
        lhs = ast_base(b);
        break;
      }
    case Token_Type_Pointer: {
        parser_advance(parser);
        Ast_Expr_Deref *d = ast_new(Ast_Expr_Deref, start_location, allocator);
        d->lhs = lhs;
        lhs = ast_base(d);
        break;
      }
    case Token_Type_Open_Square: {
        Ast_Expr_Index *i = ast_new(Ast_Expr_Index, start_location, allocator);
        i->base = lhs;
        parser_advance(parser);
        i->index = parse_expr(parser, allocator);
        parser_expect(parser, Token_Type_Close_Square, nil);
        lhs = ast_base(i);
        break;
      }
    case Token_Type_Open_Paren: {
        Ast_Expr_Call *c = ast_new(Ast_Expr_Call, start_location, allocator);
        c->lhs = lhs;
        parser_advance(parser);
        vector_init(&c->args, 0, 8, allocator);
        while (parser_peek(parser).type != Token_Type_Close_Paren) {
          Ast_Expr *e = parse_expr(parser, allocator);
          vector_append(&c->args, e);

          if (parser_peek(parser).type != Token_Type_Comma) {
            break;
          }
          parser_advance(parser);
        }
        parser_expect(parser, Token_Type_Close_Paren, nil);
        lhs = ast_base(c);
        break;
      }
    case Token_Type_Dot: {
        parser_advance(parser);
        Ast_Expr_Selector *s = ast_new(Ast_Expr_Selector, start_location, allocator);
        s->lhs = lhs;
        Token ident = {0};
        parser_expect(parser, Token_Type_Ident, &ident);
        s->selector = ident.literal.string;
        lhs = ast_base(s);
        break;
      }
    default:
      return lhs;
    }
  }
}

internal Ast_Expr *parse_expr(Parser *parser, Allocator allocator) {
  return _parse_expr(parser, 0, allocator);
}

internal void parse_block(Parser *parser, Ast_Stmt_Block *block, Allocator allocator) {
  while (parser_peek(parser).type != Token_Type_Close_Squirly) {
    Ast_Stmt *s = parse_stmt(parser, allocator);
    if (!s) {
      break;
    }
    vector_append(block, s);
  }
}

internal b8 parse_fields(Parser *parser, Token_Type terminator, Field_Vector *fields, b8 allow_defaults, Allocator allocator);

internal Ast_Stmt *parse_stmt(Parser *parser, Allocator allocator) {
  Token t = parser_peek(parser);

  switch (t.type) {
  case Token_Type_Return: {
      parser_skip(parser);
      Ast_Stmt_Return *r = ast_new(Ast_Stmt_Return, t.location, allocator);
      vector_init(&r->values, 0, 8, allocator);
      while (parser_peek(parser).type != Token_Type_Semicolon) {
        Ast_Expr *expr = parse_expr(parser, allocator);
        vector_append(&r->values, expr);

        if (parser_peek(parser).type != Token_Type_Comma) {
          break;
        }
        parser_advance(parser);
      }
      parser_expect(parser, Token_Type_Semicolon, nil);
      return ast_base(r);
    }
  case Token_Type_Continue: {
      parser_skip(parser);
      Ast_Stmt_Continue *c = ast_new(Ast_Stmt_Continue, t.location, allocator);
      t = parser_advance(parser);
      switch (t.type) {
      case Token_Type_Semicolon: {
        }
      case Token_Type_Ident: {
          c->label = t.literal.string;
        }
      default:
        errorfc(
          Error_Type_Syntax,
          ast_base(c)->location,
          "Expected 'continue' keyword to be followed by label or semicolon, but got '%S'",
          t.lexeme
        );
        return nil;
      }
    }
  case Token_Type_Break: {
      parser_skip(parser);
      Ast_Stmt_Break *c = ast_new(Ast_Stmt_Break, t.location, allocator);
      t = parser_advance(parser);
      switch (t.type) {
      case Token_Type_Semicolon: {
        }
      case Token_Type_Ident: {
          c->label = t.literal.string;
        }
      default:
        errorfc(
          Error_Type_Syntax,
          ast_base(c)->location,
          "Expected 'break' keyword to be followed by label or semicolon, but got '%S'",
          t.lexeme
        );
        return nil;
      }
    }
  case Token_Type_Defer: {
      parser_skip(parser);
      Ast_Stmt_Defer *defer = ast_new(Ast_Stmt_Defer, t.location, allocator);
      defer->rhs = parse_stmt(parser, allocator);
      return ast_base(defer);
    }
  case Token_Type_Ident:
      if (parser_peek_n(parser, 1).type == Token_Type_Colon) { // comma 
        parser_advance(parser);
        parser_expect(parser, Token_Type_Colon, nil);

        Ast_Decl_Variable *var = ast_new(Ast_Decl_Variable, t.location, allocator);

        var->name = t.literal.string;
        var->type = parse_type(parser, allocator);

        if (parser_peek(parser).type == Token_Type_Assign) {
          parser_advance(parser);
          var->value = parse_expr(parser, allocator);
        } else if (parser_peek(parser).type == Token_Type_Colon) {
          parser_advance(parser);
          var->value    = parse_expr(parser, allocator);
          var->constant = true;
        }
        parser_expect(parser, Token_Type_Semicolon, nil);

        return ast_base(var);
      }
  case Token_Type_Open_Paren: {
      Ast_Expr *lhs = parse_expr(parser, allocator);
      switch (parser_peek(parser).type) {
      case Token_Type_Assign: {
          Ast_Stmt_Assign *assign = ast_new(Ast_Stmt_Assign, t.location, allocator);
          assign->lhs = lhs;
          parser_expect(parser, Token_Type_Assign, nil);
          assign->rhs = parse_expr(parser, allocator);

          parser_expect(parser, Token_Type_Semicolon, nil);

          return ast_base(assign);
        }
      case Token_Type_Assign_Op: {
          Ast_Stmt_Assign *assign = ast_new(Ast_Stmt_Assign, t.location, allocator);
          assign->lhs = lhs;
          Token op;
          parser_expect(parser, Token_Type_Assign_Op, &op);
          Ast_Expr_Binary *b = ast_new(Ast_Expr_Binary, t.location, allocator);
          b->lhs = lhs;
          b->rhs = parse_expr(parser, allocator);
          b->op  = op.assign_op;
          assign->rhs = ast_base(b);

          parser_expect(parser, Token_Type_Semicolon, nil);

          return ast_base(assign);
        }
      case Token_Type_Semicolon: { // maybe we should have an expression statement ast node
          parser_advance(parser);
          return ast_base(lhs);
        }
      default:
        unreachable();
      }
      unreachable();
    }
  case Token_Type_If: {
      parser_expect(parser, Token_Type_If, nil);
      Ast_Stmt_If *i = ast_new(Ast_Stmt_If, t.location, allocator);
      i->cond = parse_expr(parser, allocator);
      i->body.allocator = allocator;

      parser_expect(parser, Token_Type_Open_Squirly, nil);
      parse_block(parser, &i->body, allocator);
      parser_expect(parser, Token_Type_Close_Squirly, nil);

      return ast_base(i);
    }
  case Token_Type_For: {
      parser_expect(parser, Token_Type_For, nil);
      if (parser_peek(parser).type == Token_Type_Open_Square) {
        parser_advance(parser);
        Ast_Stmt_Loop *l = ast_new(Ast_Stmt_Loop, t.location, allocator);
        Ast_Expr      *e = parse_expr(parser, allocator);
        if (parser_peek(parser).type == Token_Type_Comma) {
          parser_advance(parser);
          l->begin = e;
          l->end   = parse_expr(parser, allocator);
        } else {
          l->end = e;
        }
        parser_expect(parser, Token_Type_Close_Square, nil);
        parser_expect(parser, Token_Type_Arrow, nil);
        Token ident;
        parser_expect(parser, Token_Type_Ident, &ident);
        Ast_Decl_Variable *var = ast_new(Ast_Decl_Variable, ident.location, allocator);
        var->name  = ident.literal.string;
        parser_expect(parser, Token_Type_Colon, nil);
        var->type  = parse_type(parser, allocator);
        l->decl    = ast_base(var);

        parser_expect(parser, Token_Type_Open_Squirly, nil);
        l->body.allocator = allocator;
        parse_block(parser, &l->body, allocator);
        parser_expect(parser, Token_Type_Close_Squirly, nil);
        
        return ast_base(l);
      } else {
        Ast_Stmt_Iterator *i = ast_new(Ast_Stmt_Iterator, t.location, allocator);
        i->lhs = parse_expr(parser, allocator);
        i->body.allocator = allocator;
        i->decls.allocator = allocator;

        parser_expect(parser, Token_Type_Arrow, nil);
        parse_fields(parser, Token_Type_Open_Squirly, &i->decls, false, allocator);
        parse_block(parser, &i->body, allocator);
        parser_expect(parser, Token_Type_Close_Squirly, nil);

        return ast_base(i);
      }
    }
  case Token_Type_Fn:
  case Token_Type_Def:
    return parse_decl(parser, allocator);

  default: {
      parser_skip(parser);
      errorfc(Error_Type_Syntax, t.location, "Unexpected token at beginning of statement: '%S'", t.lexeme);
      return nil;
    }
  }

  unreachable();
}

internal Ast_Expr *parse_type(Parser *parser, Allocator allocator) {
  Token t = parser_advance(parser);
  switch (t.type) {
  case Token_Type_Open_Square: {
    Ast_Type_Array *array = ast_new(Ast_Type_Array, t.location, allocator);

    t = parser_peek(parser);
    if (t.type != Token_Type_Close_Square) {
      array->count = parse_expr(parser, allocator);
    }
    parser_expect(parser, Token_Type_Close_Square, nil);
    array->elem = parse_type(parser, allocator);
    return ast_base(array);
  }
  case Token_Type_Struct: {
    Ast_Type_Struct *s = ast_new(Ast_Type_Struct, t.location, allocator);
    vector_init(&s->fields, 0, 8, allocator);

    if (!parser_expect(parser, Token_Type_Open_Squirly, nil)) {
      break;
    }
    parse_fields(parser, Token_Type_Close_Squirly, &s->fields, false, allocator);

    return ast_base(s);
  }
  case Token_Type_Union: {
    Ast_Type_Union *s = ast_new(Ast_Type_Union, t.location, allocator);
    vector_init(&s->variants, 0, 8, allocator);

    if (!parser_expect(parser, Token_Type_Open_Squirly, nil)) {
      break;
    }
    parse_fields(parser, Token_Type_Close_Squirly, &s->variants, false, allocator);

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
    parse_fields(parser, Token_Type_Close_Paren, &args, false, allocator);

    Field_Vector returns = {0};
    Token t2 = parser_peek(parser);
    if (t2.type == Token_Type_Arrow) {
      parser_advance(parser);
      parser_expect(parser, Token_Type_Open_Paren, nil);
      returns = vector_make(Field_Vector, 0, 8, allocator);
      parse_fields(parser, Token_Type_Close_Paren, &returns, false, allocator);
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

internal b8 parse_fields(Parser *parser, Token_Type terminator, Field_Vector *fields, b8 allow_defaults, Allocator allocator) {
  for (Token t = parser_peek(parser); t.type != terminator; t = parser_peek(parser)) {
    Token ident;
    if (!parser_expect(parser, Token_Type_Ident, &ident)) {
      return false;
    }
    i32 repeat_start = fields->len;
    while (parser_peek(parser).type == Token_Type_Comma) {
      Ast_Field *field = ast_new(Ast_Field, ident.location, allocator);
      field->name = ident.literal.string;
      field->type = nil;
      vector_append(fields, field);
      parser_advance(parser);
      if (!parser_expect(parser, Token_Type_Ident, &ident)) {
        return false;
      }
    }
    if (!parser_expect(parser, Token_Type_Colon, nil)) {
      return false;
    }
    Ast_Expr *type  = parse_type(parser, allocator);
    Ast_Expr *value = nil;

    if (allow_defaults && (parser_peek(parser).type == Token_Type_Assign)) {
      parser_advance(parser);
      value = parse_expr(parser, allocator);
    }

    for_range(i, repeat_start, fields->len) {
      (*IDX(*fields, i))->type = type;
      (*IDX(*fields, i))->value = value;
    }

    Ast_Field *field = ast_new(Ast_Field, ident.location, allocator);
    field->name  = ident.literal.string;
    field->type  = type;
    field->value = value;
    vector_append(fields, field);

    if (parser_peek(parser).type != Token_Type_Comma) {
      break;
    }
    parser_advance(parser);
  }
  parser_expect(parser, terminator, nil);
  return true;
}

internal Ast_Decl_Function *parse_function_decl(Parser *parser, Allocator allocator) {
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
  parse_fields(parser, Token_Type_Close_Paren, &args, true, allocator);

  Field_Vector returns = {0};
  Token t2 = parser_peek(parser);
  if (t2.type == Token_Type_Arrow) {
    parser_advance(parser);
    parser_expect(parser, Token_Type_Open_Paren, nil);
    returns = vector_make(Field_Vector, 0, 8, allocator);
    parse_fields(parser, Token_Type_Close_Paren, &returns, true, allocator);
  }
  parser_expect(parser, Token_Type_Open_Squirly,  nil);

  Ast_Decl_Function *fn = ast_new(Ast_Decl_Function, t.location, allocator);
  fn->name    = ident.literal.string;
  fn->args    = args;
  fn->returns = returns;
  vector_init(&fn->body, 0, 8, allocator);

  parse_block(parser, &fn->body, allocator);
  parser_expect(parser, Token_Type_Close_Squirly, nil);

  return fn;
}

internal Ast_Decl *parse_decl(Parser *parser, Allocator allocator) {
  Token t = parser_peek(parser);

  switch (t.type) {
  case Token_Type_Fn: {
      Ast_Decl_Function *func_decl = parse_function_decl(parser, allocator);
      return ast_base(func_decl);
    }
  case Token_Type_Ident: {
      parser_advance(parser);
      parser_expect(parser, Token_Type_Colon, nil);

      Ast_Decl_Variable *var = ast_new(Ast_Decl_Variable, t.location, allocator);

      var->name = t.literal.string;
      var->type = parse_type(parser, allocator);

      if (parser_peek(parser).type == Token_Type_Assign) {
        parser_advance(parser);
        var->value = parse_expr(parser, allocator);
      } else if (parser_peek(parser).type == Token_Type_Colon) {
        parser_advance(parser);
        var->value    = parse_expr(parser, allocator);
        var->constant = true;
      }
      parser_expect(parser, Token_Type_Semicolon, nil);

      return ast_base(var);
    }
  case Token_Type_Def: {
      parser_skip(parser);
      Token ident;
      if (!parser_expect(parser, Token_Type_Ident, &ident)) {
        return nil;
      }
      Ast_Expr *type = parse_type(parser, allocator);
      Ast_Decl_Type *type_decl = ast_new(Ast_Decl_Type, t.location, allocator);
      type_decl->type = type;
      type_decl->name = ident.literal.string;
      parser_expect(parser, Token_Type_Semicolon, &ident);

      return ast_base(type_decl);
    }
  case Token_Type_Import: {
      parser_skip(parser);
      Token t2;
      if (!parser_expect(parser, Token_Type_String, &t2)) {
        return nil;
      }
      Ast_Decl_Import *import = ast_new(Ast_Decl_Import, t.location, allocator);
      import->path = t2.literal.string;
      if (!parser_expect(parser, Token_Type_Semicolon, &t2)) {
        return nil;
      }
      return ast_base(import);
    }
  case Token_Type_Semicolon:
  case Token_Type_EOF:
    return nil;

  default:
    parser_skip(parser);
    errorfc(Error_Type_Syntax, t.location, "Unexpected token in declaration: '%S'", t.lexeme);
    return nil;
  }

  unreachable();
}

internal void parse_file(Parser *parser, Allocator allocator) {
  while (parser->current < parser->tokens.len) {
    Ast_Decl *decl = parse_decl(parser, allocator);
    vector_append(&parser->file.decls, decl);
  }
}

internal void print_fields(Field_Vector fields) {
  vector_iter_v(fields, field, i, {
    if (i) {
      fmt_printc(", ");
    }
    fmt_printfc("%S: ", field->name);
    print_type(field->type);
    if (field->value) {
      fmt_printc(" = ");
      print_expr(field->value);
    }
  });
}

internal void print_decl(Ast_Decl *d) {
  switch (d->ast_type) {
  case ANT_Decl_Import: {
      fmt_printflnc("import \"%S\";", d->decl_import->path);
      return;
    }
  case ANT_Decl_Function: {
      fmt_printfc("fn %S(", d->decl_function->name);
      print_fields(d->decl_function->args);
      fmt_printc(") -> (");
      print_fields(d->decl_function->returns);
      fmt_printlnc(") {");
      slice_iter_v(d->decl_function->body, stmt, i, {
        fmt_printc("\t");
        print_stmt(stmt);
        fmt_printlnc(";");
      })
      fmt_printlnc("}");
      return;
    }
  case ANT_Decl_Type: {
      fmt_printfc("type %S ", d->decl_type->name);
      print_type(d->decl_type->type);
      fmt_printlnc(";");
      return;
    }
  case ANT_Decl_Variable: {
      fmt_printfc("%S: ", d->decl_variable->name);
      print_type(d->decl_variable->type);
      if (d->decl_variable->value) {
        if (d->decl_variable->constant) {
          fmt_printc(" : ");
        } else {
          fmt_printc(" = ");
        }
        print_expr(d->decl_variable->value);
      }
      fmt_printlnc(";");
      return;
    }
  default:
    unreachable();
  }
}

internal void print_stmt(Ast_Stmt *s) {
  switch (s->ast_type) {
  case ANT_Expr_Ident:
  case ANT_Expr_Literal:
  case ANT_Expr_Unary:
  case ANT_Expr_Binary:
  case ANT_Expr_Ternary:
  case ANT_Expr_Call:
  case ANT_Expr_Deref:
  case ANT_Expr_Cast:
  case ANT_Expr_Index:
  case ANT_Expr_Address:
    print_expr(s);
    return;

  case ANT_Decl_Import:
  case ANT_Decl_Function:
  case ANT_Decl_Type:
  case ANT_Decl_Variable:
    print_decl(s);
    return;

  case ANT_Stmt_Defer: {
      fmt_printc("defer ");
      print_stmt(s->stmt_defer->rhs);
      return;
    }
  case ANT_Stmt_Return: {
      fmt_printc("return ");
      slice_iter(s->stmt_return->values, r, i, {
        if (i) {
          fmt_printc(", ");
        }
        print_expr(*r);
      })
      return;
    }
  case ANT_Stmt_Break: {
      fmt_printc("break");
      return;
    }
  case ANT_Stmt_Continue: {
      fmt_printc("continue");
      return;
    }
  case ANT_Stmt_Block: {
      fmt_printc("block");
      return;
    }
  case ANT_Stmt_If: {
      fmt_printc("if ");
      print_expr(s->stmt_if->cond);
      fmt_printlnc(" {");
      slice_iter_v(s->stmt_if->body, stmt, i, {
        fmt_printc("\t");
        print_stmt(stmt);
        fmt_printlnc(";");
      });
      fmt_printc("}");
      return;
    }
  case ANT_Stmt_Loop: {
      fmt_printc("for [");
      if (s->stmt_loop->begin) {
        print_expr(s->stmt_loop->begin);
        fmt_printc(", ");
      }
      print_expr(s->stmt_loop->end);
      fmt_printc("] -> ");
      print_decl(s->stmt_loop->decl);
      fmt_printlnc(" {");
      slice_iter_v(s->stmt_loop->body, stmt, i, {
        fmt_printc("\t");
        print_stmt(stmt);
        fmt_printlnc(";");
      });
      fmt_printc("\t}");
      return;
    }
  case ANT_Stmt_Switch: {
      fmt_printc("switch");
      return;
    }
  case ANT_Stmt_Iterator: {
      fmt_printc("for ");
      print_expr(s->stmt_iterator->lhs);
      fmt_printc(" -> ");
      slice_iter_v(s->stmt_iterator->decls, decl, i, {
         if (i) {
            fmt_printc(", ");
         }
        fmt_printfc("%S: ", decl->name);
        print_type(decl->type);
      });
      fmt_printlnc(" {");
      slice_iter_v(s->stmt_iterator->body, stmt, i, {
        fmt_printc("\t");
        print_stmt(stmt);
        fmt_printlnc(";");
      });
      fmt_printc("\t}");
      return;
    }
  case ANT_Stmt_Assign: {
      print_expr(s->stmt_assign->lhs);
      fmt_printc(" = ");
      print_expr(s->stmt_assign->rhs);

      return;
    }
  default:
    unreachable();
  }
}

internal void print_expr(Ast_Expr *e) {
  fmt_printc("(");
  switch (e->ast_type) {
  case ANT_Expr_Selector: {
      print_expr(e->expr_selector->lhs);
      fmt_printfc(".%S", e->expr_selector->selector);
      break;
    }
  case ANT_Expr_Ident: {
      fmt_print(e->expr_ident->name);
      break;
    }
  case ANT_Expr_Literal: {
      switch (e->expr_literal->type) {
      case Token_Type_String: {
          fmt_printfc("\"%S\"", e->expr_literal->value.string);
          break;
        }
      case Token_Type_Int: {
          fmt_printfc("%d", e->expr_literal->value.integer);
          break;
        }
      case Token_Type_Rune: {
          fmt_printfc("'%c'", e->expr_literal->value.rune);
          break;
        }
      case Token_Type_Float: {
          fmt_printfc("%d", e->expr_literal->value.decimal);
          break;
        }
      default:
        unreachable();
      }
      break;
    }
  case ANT_Expr_Unary: {
      slice_iter_v(c_array_to_slice(single_char_tokens), single_char_token, i, {
        if (e->expr_unary->op == single_char_token) {
          fmt_printfc("%c", i);
        }
      });
      print_expr(e->expr_unary->rhs);
      break;
    }
  case ANT_Expr_Binary: {
      print_expr(e->expr_binary->lhs);
      fmt_printfc(" %S ", binary_op_strings[e->expr_binary->op]);
      print_expr(e->expr_binary->rhs);
      break;
    }
  case ANT_Expr_Ternary: {
      
      break;
    }
  case ANT_Expr_Call: {
      print_expr(e->expr_call->lhs);
      fmt_printc("(");
      slice_iter_v(e->expr_call->args, arg, i, {
        if (i) {
          fmt_printc(", ");
        }
        print_expr(arg);
      });
      fmt_printc(")");

      break;
    }
  case ANT_Expr_Deref: {
      print_expr(e->expr_deref->lhs);
      fmt_printc("^");
      
      break;
    }
  case ANT_Expr_Cast: {
      fmt_printc("cast(");
      print_type(e->expr_cast->type);
      fmt_printc(")");
      print_expr(e->expr_cast->rhs);

      break;
    }
  case ANT_Expr_Index: {
      print_expr(e->expr_index->base);
      fmt_printc("[");
      print_expr(e->expr_index->index);
      fmt_printc("]");

      break;
    }
  default:
    unreachable();
  }
  fmt_printc(")");
}

internal void print_type(Ast_Expr *t) {
  switch (t->ast_type) {
  case ANT_Type_Function: {
    fmt_printc("fn(");
    print_fields(t->type_function->args);
    fmt_printc(") -> (");
    print_fields(t->type_function->returns);
    fmt_printc(")");
    break;
  }
  case ANT_Type_Pointer: {
    fmt_printc("^");
    print_type(t->type_pointer->pointee);
    break;
  }
  case ANT_Type_Array: {
    fmt_printc("[");
    if (t->type_array->count) {
      print_expr(t->type_array->count);
    }
    fmt_printc("]");
    print_type(t->type_array->elem);
    break;
  }
  case ANT_Type_Enum: {
    fmt_printc("enum { ");
    vector_iter_v(t->type_enum->values, value, _i , {
      fmt_printfc("%S, ", value.name);
    });
    fmt_printc(" }");
    break;
  }
  case ANT_Type_Struct: {
    fmt_printc("struct { ");
    print_fields(t->type_struct->fields);
    fmt_printc(" }");
    break;
  }
  case ANT_Type_Union: {
    fmt_printc("union { ");
    print_fields(t->type_union->variants);
    fmt_printc(" }");
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

internal Allocator_Result printing_allocator_proc(
  rawptr               data,
  Allocator_Mode       mode,
  isize                size,
  isize                align,
  rawptr               old_memory,
  Source_Code_Location location
) {
  if (mode == AM_Alloc) {
    CONTEXT_GUARD({
      static byte backing[256];
      Arena_Allocator a = {0};
      context.temp_allocator = arena_allocator_init(&a, transmute(Byte_Slice, c_array_to_slice(backing)));
      fmt_printfln(LIT("Allocation: %M\t at %L"), size, location);
    });
  }
  return growing_arena_allocator_proc(
    data,
    mode,
    size,
    align,
    old_memory,
    location
  );
}

#define TYPE_KINDS(X)                                                          \
  X(Type_Kind_Array)                                                           \
  X(Type_Kind_Struct)                                                          \
  X(Type_Kind_Union)                                                           \
  X(Type_Kind_Enum)                                                            \
  X(Type_Kind_Named)                                                           \
  X(Type_Kind_Pointer)                                                         \
  X(Type_Kind_Basic)                                                           \
  X(Type_Kind_Function)                                                        \
  X(Type_Kind_Untyped)

X_ENUM(Type_Kind, TYPE_KINDS);

#define BASIC_TYPES(X)                                                         \
  X(Basic_Type_Int)                                                            \
  X(Basic_Type_Bool)                                                           \
  X(Basic_Type_Uint)                                                           \
  X(Basic_Type_Rune)                                                           \
  X(Basic_Type_Float)                                                          \
  X(Basic_Type_String)                                                         \
  X(Basic_Type_Cstring)                                                        \
  X(Basic_Type_Rawptr)                                                         \
  X(Basic_Type_Uintptr)

X_ENUM(Basic_Type_Kind, BASIC_TYPES);

#define UNTYPED_TYPES(X)                                                       \
  X(Untyped_Type_Int)                                                          \
  X(Untyped_Type_Bool)                                                         \
  X(Untyped_Type_Float)                                                        \
  X(Untyped_Type_String)                                                       \
  X(Untyped_Type_Rune)

X_ENUM(Untyped_Type_Kind, UNTYPED_TYPES);

typedef struct _Type Type;

typedef struct {
  Slice(struct {
    String name;
    Type  *type;
    isize  offset;
  }) fields;
} Type_Struct;

typedef struct {
  Slice(struct {
    String name;
    Type  *type;
  }) variants;
} Type_Union;

typedef struct {
  Slice(struct {
    String name;
    isize  value;
  }) values;
  Type *backing;
} Type_Enum;

typedef struct {
  Type *elem;
  isize count;
} Type_Array;

typedef struct {
  Type *elem;
} Type_Slice;

typedef struct {
  Type *pointee;
} Type_Pointer;

typedef struct {
  Type *type;
} Type_Named;

typedef struct {
  Basic_Type_Kind basic_kind;
} Type_Basic;

typedef struct {
  Untyped_Type_Kind untyped_kind;
} Type_Untyped;

typedef struct {
  Slice(struct {
    String name;
    Type  *type;
  }) args;
  Slice(struct {
    String name;
    Type  *type;
  }) returns;
} Type_Function;

struct _Type {
  Type_Kind type_kind;
  isize     size, align;

  union {
    Type_Struct   struct_ [0];
    Type_Array    array   [0];
    Type_Pointer  pointer [0];
    Type_Union    union_  [0];
    Type_Enum     enum_   [0];
    Type_Named    named   [0];
    Type_Function function[0];
    Type_Untyped  untyped [0];

    rawptr _union_accessor[0];
  };
};

#define type_new(T, allocator)                                                 \
  ({                                                                           \
    Type *new_type = (Type *)(unwrap_err(mem_alloc_aligned(                    \
        size_of(Type) + size_of(Type_##T), 8, (allocator))));                  \
    new_type->type_kind = Type_Kind_##T;                                       \
    (Type_##T *)&new_type->_union_accessor;                                    \
  })

#define type_base(type, _size, _align) ({                                      \
    ((Type *)type)[-1].align = (_align);                                       \
    ((Type *)type)[-1].size  = (_size);                                        \
    &((Type *)type)[-1];                                                       \
  })

typedef struct _Checker_Scope {
  Hash_Map(String, Type *) types;
  Hash_Map(String, Type *) variables;
  struct _Checker_Scope   *parent;
} Checker_Scope;

internal Type *resolve_named_type(Checker_Scope *scope, String name) {
  while (scope) {
    Type **t = hash_map_get(scope->types, name);
    if (t) {
      return *t;
    }
    scope = scope->parent;
  }
  return nil;
}

internal Type *resolve_ident(Checker_Scope *scope, String name) {
  while (scope) {
    Type **t = hash_map_get(scope->variables, name);
    if (t) {
      return *t;
    }
    scope = scope->parent;
  }
  return nil;
}

internal Type *resolve_ast_type(Ast_Expr *t, Checker_Scope *scope, Allocator allocator) {
  SWITCH (t->ast_type) {
  CASE ANT_Type_Function: {
    Type_Function *fn = type_new(Function, allocator);
    slice_init(&fn->args,    t->type_function->args.len,    allocator);
    slice_init(&fn->returns, t->type_function->returns.len, allocator);
    slice_iter_v(t->type_function->args, arg, i, {
      Type *arg_type = resolve_ast_type(arg->type, scope, allocator);
      IDX(fn->args, i)->type = arg_type;
      IDX(fn->args, i)->name = arg->name;
    });
    slice_iter_v(t->type_function->returns, ret, i, {
      Type *ret_type = resolve_ast_type(ret->type, scope, allocator);
      IDX(fn->returns, i)->type = ret_type;
      IDX(fn->returns, i)->name = ret->name;
    });
    return type_base(fn, 8, 8);
  }
  CASE ANT_Type_Pointer: {
    Type_Pointer *p = type_new(Pointer, allocator);
    p->pointee = resolve_ast_type(t->type_pointer->pointee, scope, allocator);
    return type_base(p, 8, 8);
  }
  CASE ANT_Type_Array: {
    Type_Array *a = type_new(Array, allocator);
    a->elem = resolve_ast_type(t->type_array->elem, scope, allocator);
    return type_base(a, a->elem->size, a->elem->align);
  }
  CASE ANT_Type_Enum: {
    Type_Enum *e = type_new(Enum, allocator);
    slice_init(&e->values, t->type_enum->values.len, allocator);
    slice_iter_v(t->type_enum->values, value, i, {
      IDX(e->values, i)->name  = value.name;
      IDX(e->values, i)->value = i;
    });
    return type_base(e, 8, 8);
  }
  CASE ANT_Type_Struct: {
    Type_Struct *s = type_new(Struct, allocator);
    slice_init(&s->fields, t->type_struct->fields.len, allocator);
    isize offset    = 0;
    isize max_align = 0;
    slice_iter_v(t->type_struct->fields, field, i, {
      Type *field_type = resolve_ast_type(field->type, scope, allocator);
      offset = (offset + field_type->align - 1) & ~(field_type->align - 1);

      IDX(s->fields, i)->name   = field->name;
      IDX(s->fields, i)->type   = field_type;
      IDX(s->fields, i)->offset = offset;

      offset   += IDX(s->fields, i)->type->size;
      max_align = max(max_align, IDX(s->fields, i)->type->align);
    });
    offset = (offset + max_align - 1) & ~(max_align - 1);
    return type_base(s, offset, max_align);
  }
  CASE ANT_Type_Union: {
    Type_Union *u = type_new(Union, allocator);
    slice_init(&u->variants, t->type_union->variants.len, allocator);
    isize max_align = 0, max_size = 0;
    slice_iter_v(t->type_union->variants, variant, i, {
      IDX(u->variants, i)->name = variant->name;
      IDX(u->variants, i)->type = resolve_ast_type(variant->type, scope, allocator);

      max_align = max(max_align, IDX(u->variants, i)->type->align);
      max_size  = max(max_size,  IDX(u->variants, i)->type->size );
    });
    return type_base(u, max_size, max_align);
  }
  CASE ANT_Type_Named: {
    Type_Named *n = type_new(Named, allocator);
    n->type       = resolve_named_type(scope, t->type_named->name);
    if (n->type) {
      return type_base(n, n->type->size, n->type->align);
    }
    return type_base(n, 0, 0);
  }
  DEFAULT:
    unreachable();
  }
  unreachable();
}

internal b8 typecheck_function(Ast_Decl_Function function) {
  return true;
}

internal Type *new_basic_type(Basic_Type_Kind kind, isize size, isize align, Allocator allocator) {
  Type_Basic *b = type_new(Basic, allocator);
  b->basic_kind = kind;
  return type_base(b, size, align);
}

internal Type_Function *type_check_function_decl(Ast_Decl_Function *function, Checker_Scope *cs, Allocator allocator) {
  Type_Function *f = type_new(Function, allocator);
  slice_init(&f->args,    function->args.len,    allocator);
  slice_init(&f->returns, function->returns.len, allocator);

  Checker_Scope scope = {
    .parent = cs,
  };
  scope.variables.allocator = allocator;
  scope.types.allocator = allocator;

  slice_iter_v(function->args, arg, i, {
    f->args.data[i].type = resolve_ast_type(arg->type, cs, allocator);
    f->args.data[i].name = arg->name;

    hash_map_insert(&scope.variables, f->args.data[i].name, f->args.data[i].type);
  });
  slice_iter_v(function->returns, ret, i, {
    f->returns.data[i].type = resolve_ast_type(ret->type, cs, allocator);
    f->returns.data[i].name = ret->name;

    hash_map_insert(&scope.variables, f->returns.data[i].name, f->returns.data[i].type);
  });

  hash_map_iter(scope.types, name, type, {
    fmt_printflnc("%S:\tType: %12S, Size: %2d, Align: %2d", function->name, name, (*type)->size, (*type)->align);
  });

  hash_map_iter(scope.variables, name, type, {
    fmt_printflnc("%S:\tVar:  %12S, Size: %2d, Align: %2d", function->name, name, (*type)->size, (*type)->align);
  });

  return f;
}

internal void type_check_variable_decl(Ast_Decl_Variable *variable, Checker_Scope *cs, Allocator allocator) {
}

internal Type *type_base_type(Type *type) {
  while (type->type_kind == Type_Kind_Named) {
    type = type->named->type;
  }
  return type;
}

internal Type *type_check_expr(Ast_Expr *expr, Checker_Scope *cs, Allocator allocator) {
  switch (expr->ast_type) {
  CASE ANT_Expr_Ident:
    return resolve_ident(cs, expr->expr_ident->name);
  CASE ANT_Expr_Literal: {
    Type_Untyped *u = type_new(Untyped, allocator);
    switch (expr->expr_literal->type) {
    CASE Token_Type_String:
      u->untyped_kind = Untyped_Type_String;
    CASE Token_Type_Int:
      u->untyped_kind = Untyped_Type_Int;
    CASE Token_Type_Float:
      u->untyped_kind = Untyped_Type_Float;
    CASE Token_Type_Rune:
      u->untyped_kind = Untyped_Type_Rune;
    DEFAULT:
      unreachable();
    }
    return type_base(u, 0, 0);
  }
  CASE ANT_Expr_Unary:
    return type_check_expr(expr, cs, allocator);
  CASE ANT_Expr_Binary:
  CASE ANT_Expr_Ternary:
  CASE ANT_Expr_Call:
  CASE ANT_Expr_Selector:
  CASE ANT_Expr_Deref:
  CASE ANT_Expr_Cast:
  CASE ANT_Expr_Index:
  CASE ANT_Expr_Address:
  DEFAULT: {}
  }
  return nil;
}

internal b8 type_check_file(Ast_File file, Allocator allocator) {
  b8 ok = true;

  Checker_Scope cs = {0};
  hash_map_init(&cs.variables, 64, string_equal, string_hash, allocator);
  hash_map_init(&cs.types    , 64, string_equal, string_hash, allocator);

  // TODO(Franz): set int, uint and pointer types to platform specific values

  #define BASIC_TYPE(name, size, align)                                          \
    {                                                                            \
      Type *t = new_basic_type(Basic_Type_Int, size, align, allocator);          \
      hash_map_insert(&cs.types, LIT(name), t);                                  \
    }

  BASIC_TYPE("i8",      1, 1);
  BASIC_TYPE("i16",     2, 2);
  BASIC_TYPE("i32",     4, 4);
  BASIC_TYPE("i64",     8, 8);
  BASIC_TYPE("int",     8, 8);

  BASIC_TYPE("b8",      1, 1);
  BASIC_TYPE("b16",     2, 2);
  BASIC_TYPE("b32",     4, 4);
  BASIC_TYPE("b64",     8, 8);
  BASIC_TYPE("bool",    1, 1);

  BASIC_TYPE("u8",      1, 1);
  BASIC_TYPE("u16",     2, 2);
  BASIC_TYPE("u32",     4, 4);
  BASIC_TYPE("u64",     8, 8);
  BASIC_TYPE("uint",    8, 8);

  BASIC_TYPE("f32",     4, 4);
  BASIC_TYPE("f64",     8, 8);

  BASIC_TYPE("string", 16, 8);
  BASIC_TYPE("cstring", 8, 8);

  BASIC_TYPE("rune",    4, 4);

  BASIC_TYPE("rawptr",  8, 8);
  BASIC_TYPE("uintptr", 8, 8);

  #undef BASIC_TYPE
  
  slice_iter_v(file.decls, decl, _i, {
    switch (decl->ast_type) {
    CASE ANT_Decl_Type: {
      Type *t = resolve_ast_type(decl->decl_type->type, &cs, allocator);
      hash_map_insert(&cs.types, decl->decl_type->name, t);
    }
    DEFAULT: {}
    }
  });

  slice_iter_v(file.decls, decl, _i, {
    switch (decl->ast_type) {
    CASE ANT_Decl_Function: {
      type_check_function_decl(decl->decl_function, &cs, allocator);
    }
    CASE ANT_Decl_Variable: {
      Type *variable_type = resolve_ast_type(decl->decl_variable->type, &cs, allocator);
      hash_map_insert(&cs.variables, decl->decl_variable->name, variable_type);
    }
    DEFAULT: {}
    }
  });

  hash_map_iter(cs.types, name, type, {
    fmt_printflnc("Type: %12S, Size: %2d, Align: %2d", name, (*type)->size, (*type)->align);
  });

  hash_map_iter(cs.variables, name, type, {
    fmt_printflnc("Var:  %12S, Size: %2d, Align: %2d", name, (*type)->size, (*type)->align);
  });

  return ok;
}

i32 main() {
  context.allocator = panic_allocator();
  context.logger    = create_file_logger(FD_STDERR);

  struct Time start = time_now();

  // context.temp_allocator.proc = printing_allocator_proc;

  if (os_args.len < 2) {
    fmt_eprintflnc("Usage: %S <file> <flags>", os_args.data[0]);
    os_exit(1);
  }

  isize allocated = 0;

  slice_iter_v(slice_start(os_args, 1), path, _i, {
    Byte_Slice file_data = or_do_err(read_entire_file_path(path, context.temp_allocator), err, {
      fmt_eprintflnc("Failed to read file '%S': '%S'", path, enum_to_string(OS_Error, err));
      continue;
    });
    Token_Vector tokens = vector_make(Token_Vector, 0, 8, context.temp_allocator);
    if (!tokenize_file(bytes_to_string(file_data), path, &tokens)) {
      fmt_eprintflnc("Failed to tokenize file '%S'", path);
      continue;
    }

    allocated = 0;
    slice_iter_v(__default_temp_allocator_arena.blocks, block, i, {
      allocated += block.used;
    })
    fmt_printflnc("Bytes allocated: %M", allocated);

    Parser parser    = {0};
    parser.tokens    = vector_to_slice(type_of(parser.tokens), tokens);
    parser.file.path = path;
    vector_init(&parser.file.decls, 0, 8, context.temp_allocator);

    parse_file(&parser, context.temp_allocator);

    allocated = 0;
    slice_iter_v(__default_temp_allocator_arena.blocks, block, i, {
      allocated += block.used;
    })
    fmt_printflnc("Bytes allocated: %M", allocated);

    // slice_iter_v(parser.file.decls, d, _i, print_decl(d));

    if (!type_check_file(parser.file, context.temp_allocator)) {
      fmt_eprintflnc("Type checking failed for file: '%S'", path);
      continue;
    }

    allocated = 0;
    slice_iter_v(__default_temp_allocator_arena.blocks, block, i, {
      allocated += block.used;
    })
    fmt_printflnc("Bytes allocated: %M", allocated);
  });

  log_infof(LIT("time: %fms"), (f32)time_since(start) / Millisecond);

  return 0;
}
