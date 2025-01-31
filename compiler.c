#include "codin.h"

#include "spall.h"

thread_local SpallBuffer  spall_buffer;
thread_local SpallProfile spall_ctx;

internal f64 get_time_in_micros() {
  return (f64)(time_now().nsec) / Microsecond;
}

#define TOKENS(X)                                                              \
  X(Token_Type_String       )                                                  \
  X(Token_Type_Int          )                                                  \
  X(Token_Type_Float        )                                                  \
  X(Token_Type_Rune         )                                                  \
  X(Token_Type_Ident        )                                                  \
                                                                               \
  X(Token_Type_True         )                                                  \
  X(Token_Type_False        )                                                  \
                                                                               \
  X(Token_Type_Assign       )                                                  \
  X(Token_Type_Assign_Op    )                                                  \
  X(Token_Type_Semicolon    )                                                  \
  X(Token_Type_Colon        )                                                  \
  X(Token_Type_Comma        )                                                  \
  X(Token_Type_Arrow        )                                                  \
  X(Token_Type_Dot          )                                                  \
  X(Token_Type_Dollar       )                                                  \
  X(Token_Type_Pointer      )                                                  \
                                                                               \
  X(Token_Type_Plus         )                                                  \
  X(Token_Type_Minus        )                                                  \
  X(Token_Type_Multiply     )                                                  \
  X(Token_Type_Divide       )                                                  \
  X(Token_Type_Not          )                                                  \
  X(Token_Type_And          )                                                  \
  X(Token_Type_Or           )                                                  \
  X(Token_Type_Bit_And      )                                                  \
  X(Token_Type_Bit_Or       )                                                  \
  X(Token_Type_Less         )                                                  \
  X(Token_Type_More         )                                                  \
  X(Token_Type_Equal        )                                                  \
  X(Token_Type_Less_Equal   )                                                  \
  X(Token_Type_More_Equal   )                                                  \
  X(Token_Type_Modulo       )                                                  \
                                                                               \
  X(Token_Type_Open_Paren   )                                                  \
  X(Token_Type_Close_Paren  )                                                  \
  X(Token_Type_Open_Squirly )                                                  \
  X(Token_Type_Close_Squirly)                                                  \
  X(Token_Type_Open_Square  )                                                  \
  X(Token_Type_Close_Square )                                                  \
                                                                               \
  X(Token_Type_Cast         )                                                  \
  X(Token_Type_Transmute    )                                                  \
                                                                               \
  X(Token_Type_If           )                                                  \
  X(Token_Type_For          )                                                  \
  X(Token_Type_Break        )                                                  \
  X(Token_Type_Return       )                                                  \
  X(Token_Type_Defer        )                                                  \
  X(Token_Type_Continue     )                                                  \
  X(Token_Type_Fn           )                                                  \
  X(Token_Type_Def          )                                                  \
  X(Token_Type_Enum         )                                                  \
  X(Token_Type_Union        )                                                  \
  X(Token_Type_Struct       )                                                  \
  X(Token_Type_Ignore       )                                                  \
  X(Token_Type_Import       )                                                  \
  X(Token_Type_Dynamic      )                                                  \
                                                                               \
  X(Token_Type_EOF          )                                                  \

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
      b8     bool;
    } literal;
    Token_Type assign_op;
  };
} Token;

typedef Vector(Token) Token_Vector;

struct {
  Token_Type type;
  String     string;
} keyword_strings[] = {
  { Token_Type_If,        LIT("if"       ), },
  { Token_Type_For,       LIT("for"      ), },
  { Token_Type_Break,     LIT("break"    ), },
  { Token_Type_Return,    LIT("return"   ), },
  { Token_Type_Defer,     LIT("defer"    ), },
  { Token_Type_Continue,  LIT("continue" ), },

  { Token_Type_True,      LIT("true"     ), },
  { Token_Type_False,     LIT("false"    ), },

  { Token_Type_Fn,        LIT("fn"       ), },
  { Token_Type_Def,       LIT("type"     ), },
  { Token_Type_Enum,      LIT("enum"     ), },
  { Token_Type_Union,     LIT("union"    ), },
  { Token_Type_Struct,    LIT("struct"   ), },
  { Token_Type_Ignore,    LIT("_"        ), },
  { Token_Type_Import,    LIT("import"   ), },
  { Token_Type_Dynamic,   LIT("dynamic"  ), },

  { Token_Type_Cast,      LIT("cast"     ), },
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

#define ERROR_TYPES(X)                                                         \
  X(Error_Type_Tokenizer)                                                      \
  X(Error_Type_Syntax   )                                                      \
  X(Error_Type_Type     )                                                      \
  X(Error_Type_Internal )                                                      \
  X(Error_Type_Linker   )                                                      \

X_ENUM(Error_Type, ERROR_TYPES);

#define errorc( type, location, msg)         error( (type), (location), (LIT(msg)))
#define errorf( type, location, format, ...) error( (type), (location), (fmt_tprintf(format, __VA_ARGS__)))
#define errorfc(type, location, format, ...) errorf((type), (location), LIT(format), __VA_ARGS__)

internal void error(Error_Type type, Token_Location location, String msg) {
  fmt_eprintflnc("\033[1;31m%S(%d:%d):\033[0m %S", location.file, location.line, location.column, msg);
}

internal b8 tokenize_file(String file, String path, Token_Vector *tokens) {
  spall_buffer_begin(&spall_ctx, &spall_buffer, LIT(__FUNCTION__), get_time_in_micros());
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
  
  spall_buffer_end(&spall_ctx, &spall_buffer, get_time_in_micros());
  return ok;
}

#define AST_NODE_TYPES(X)                                                      \
  X(ANT_Expr_Ident   )                                                         \
  X(ANT_Expr_Literal )                                                         \
  X(ANT_Expr_Unary   )                                                         \
  X(ANT_Expr_Binary  )                                                         \
  X(ANT_Expr_Ternary )                                                         \
  X(ANT_Expr_Call    )                                                         \
  X(ANT_Expr_Selector)                                                         \
  X(ANT_Expr_Deref   )                                                         \
  X(ANT_Expr_Cast    )                                                         \
  X(ANT_Expr_Index   )                                                         \
  X(ANT_Expr_Address )                                                         \
                                                                               \
  X(ANT_Type_Pointer )                                                         \
  X(ANT_Type_Array   )                                                         \
  X(ANT_Type_Named   )                                                         \
  X(ANT_Type_Struct  )                                                         \
  X(ANT_Type_Union   )                                                         \
  X(ANT_Type_Enum    )                                                         \
  X(ANT_Type_Function)                                                         \
                                                                               \
  X(ANT_Decl_Import  )                                                         \
  X(ANT_Decl_Function)                                                         \
  X(ANT_Decl_Type    )                                                         \
  X(ANT_Decl_Variable)                                                         \
                                                                               \
  X(ANT_Stmt_Defer   )                                                         \
  X(ANT_Stmt_Return  )                                                         \
  X(ANT_Stmt_Break   )                                                         \
  X(ANT_Stmt_Continue)                                                         \
  X(ANT_Stmt_Block   )                                                         \
  X(ANT_Stmt_If      )                                                         \
  X(ANT_Stmt_Loop    )                                                         \
  X(ANT_Stmt_Iterator)                                                         \
  X(ANT_Stmt_Switch  )                                                         \
  X(ANT_Stmt_Assign  )                                                         \
  X(ANT_Field        )

X_ENUM(Ast_Node_Type, AST_NODE_TYPES);

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
    b8     bool;
  } value;
} Ast_Expr_Literal;

typedef struct {
  Ast_Expr  *rhs;
} Ast_Expr_Address;

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
    Ast_Expr_Address  expr_address [0];
    
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
    Ast_Stmt_Decl     stmt_decl    [0];

    Ast_Field         field[0];

    rawptr _union_accessor[0];
  };
};

#define ast_new(Type, _location, allocator)                                    \
  ({                                                                           \
    Ast_Node *ast_new_node = (Ast_Node *)(unwrap_err(mem_alloc(                \
        size_of(Ast_Node) + size_of(Ast_##Type), (allocator))));               \
    ast_new_node->location = _location;                                        \
    ast_new_node->ast_type = ANT_##Type;                                       \
    (Ast_##Type *)&ast_new_node->_union_accessor;                              \
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

internal void print_ast_expr(Ast_Expr *e);
internal void print_ast_type(Ast_Expr *e);
internal void print_ast_stmt(Ast_Stmt *s);
internal void print_ast_decl(Ast_Decl *d);

internal Ast_Expr *parse_atom_expr(Parser *parser, Allocator allocator) {
  Token t = parser_advance(parser);

  switch (t.type) {
  case Token_Type_Ident: {
      Ast_Expr_Ident *ident = ast_new(Expr_Ident, t.location, allocator);
      ident->name = t.literal.string;
      return ast_base(ident);
    }
  case Token_Type_String: {
      Ast_Expr_Literal *literal = ast_new(Expr_Literal, t.location, allocator);
      literal->value.string = t.literal.string;
      literal->type         = t.type;
      return ast_base(literal);
    }
  case Token_Type_Int: {
      Ast_Expr_Literal *literal = ast_new(Expr_Literal, t.location, allocator);
      literal->value.integer = t.literal.integer;
      literal->type          = t.type;
      return ast_base(literal);
    }
  case Token_Type_Float: {
      Ast_Expr_Literal *literal = ast_new(Expr_Literal, t.location, allocator);
      literal->value.decimal = t.literal.decimal;
      literal->type          = t.type;
      return ast_base(literal);
    }
  case Token_Type_True:
  case Token_Type_False: {
      Ast_Expr_Literal *literal = ast_new(Expr_Literal, t.location, allocator);
      literal->value.bool    = t.type == Token_Type_True;
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
      Ast_Expr_Cast *cast = ast_new(Expr_Cast, t.location, allocator);
      cast->bitwise = t.type == Token_Type_Transmute;

      parser_expect(parser, Token_Type_Open_Paren, nil);
      cast->type = parse_type(parser, allocator);
      parser_expect(parser, Token_Type_Close_Paren, nil);
      cast->rhs = parse_expr(parser, allocator);

      return ast_base(cast);
    }
  case Token_Type_And: {
      Ast_Expr_Address *addr = ast_new(Expr_Address, t.location, allocator);
      addr->rhs = parse_expr(parser, allocator);
      return ast_base(addr);
    }
  case Token_Type_Plus:
  case Token_Type_Minus: {
      Ast_Expr_Unary *unary = ast_new(Expr_Unary, t.location, allocator);
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
        Ast_Expr_Binary *b = ast_new(Expr_Binary, start_location, allocator);
        b->op  = t.type;
        b->lhs = lhs;
        b->rhs = rhs;
        lhs = ast_base(b);
        break;
      }
    case Token_Type_Pointer: {
        parser_advance(parser);
        Ast_Expr_Deref *d = ast_new(Expr_Deref, start_location, allocator);
        d->lhs = lhs;
        lhs = ast_base(d);
        break;
      }
    case Token_Type_Open_Square: {
        Ast_Expr_Index *i = ast_new(Expr_Index, start_location, allocator);
        i->base = lhs;
        parser_advance(parser);
        i->index = parse_expr(parser, allocator);
        parser_expect(parser, Token_Type_Close_Square, nil);
        lhs = ast_base(i);
        break;
      }
    case Token_Type_Open_Paren: {
        Ast_Expr_Call *c = ast_new(Expr_Call, start_location, allocator);
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
        Ast_Expr_Selector *s = ast_new(Expr_Selector, start_location, allocator);
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
      Ast_Stmt_Return *r = ast_new(Stmt_Return, t.location, allocator);
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
      Ast_Stmt_Continue *c = ast_new(Stmt_Continue, t.location, allocator);
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
      Ast_Stmt_Break *c = ast_new(Stmt_Break, t.location, allocator);
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
      Ast_Stmt_Defer *defer = ast_new(Stmt_Defer, t.location, allocator);
      defer->rhs = parse_stmt(parser, allocator);
      return ast_base(defer);
    }
  case Token_Type_Ident:
      if (parser_peek_n(parser, 1).type == Token_Type_Colon) { // comma 
        parser_advance(parser);
        parser_expect(parser, Token_Type_Colon, nil);

        Ast_Decl_Variable *var = ast_new(Decl_Variable, t.location, allocator);

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
          Ast_Stmt_Assign *assign = ast_new(Stmt_Assign, t.location, allocator);
          assign->lhs = lhs;
          parser_expect(parser, Token_Type_Assign, nil);
          assign->rhs = parse_expr(parser, allocator);

          parser_expect(parser, Token_Type_Semicolon, nil);

          return ast_base(assign);
        }
      case Token_Type_Assign_Op: {
          Ast_Stmt_Assign *assign = ast_new(Stmt_Assign, t.location, allocator);
          assign->lhs = lhs;
          Token op;
          parser_expect(parser, Token_Type_Assign_Op, &op);
          Ast_Expr_Binary *b = ast_new(Expr_Binary, t.location, allocator);
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
      Ast_Stmt_If *i = ast_new(Stmt_If, t.location, allocator);
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
        Ast_Stmt_Loop *l = ast_new(Stmt_Loop, t.location, allocator);
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
        Ast_Decl_Variable *var = ast_new(Decl_Variable, ident.location, allocator);
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
        Ast_Stmt_Iterator *i = ast_new(Stmt_Iterator, t.location, allocator);
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
    Ast_Type_Array *array = ast_new(Type_Array, t.location, allocator);

    t = parser_peek(parser);
    if (t.type != Token_Type_Close_Square) {
      array->count = parse_expr(parser, allocator);
    }
    parser_expect(parser, Token_Type_Close_Square, nil);
    array->elem = parse_type(parser, allocator);
    return ast_base(array);
  }
  case Token_Type_Struct: {
    Ast_Type_Struct *s = ast_new(Type_Struct, t.location, allocator);
    vector_init(&s->fields, 0, 8, allocator);

    if (!parser_expect(parser, Token_Type_Open_Squirly, nil)) {
      break;
    }
    parse_fields(parser, Token_Type_Close_Squirly, &s->fields, false, allocator);

    return ast_base(s);
  }
  case Token_Type_Union: {
    Ast_Type_Union *s = ast_new(Type_Union, t.location, allocator);
    vector_init(&s->variants, 0, 8, allocator);

    if (!parser_expect(parser, Token_Type_Open_Squirly, nil)) {
      break;
    }
    parse_fields(parser, Token_Type_Close_Squirly, &s->variants, false, allocator);

    return ast_base(s);
  }
  case Token_Type_Enum: {
    Ast_Type_Enum *e = ast_new(Type_Enum, t.location, allocator);
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
    Ast_Type_Pointer *pointer = ast_new(Type_Pointer, t.location, allocator);
    pointer->pointee  = parse_type(parser, allocator);
    return ast_base(pointer);
  }
  case Token_Type_Ident: {
    Ast_Type_Named *type = ast_new(Type_Named, t.location, allocator);
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

    Ast_Type_Function *fn = ast_new(Type_Function, t.location, allocator);
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
      Ast_Field *field = ast_new(Field, ident.location, allocator);
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

    Ast_Field *field = ast_new(Field, ident.location, allocator);
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

  Ast_Decl_Function *fn = ast_new(Decl_Function, t.location, allocator);
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

      Ast_Decl_Variable *var = ast_new(Decl_Variable, t.location, allocator);

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
      Ast_Decl_Type *type_decl = ast_new(Decl_Type, t.location, allocator);
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
      Ast_Decl_Import *import = ast_new(Decl_Import, t.location, allocator);
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
  spall_buffer_begin(&spall_ctx, &spall_buffer, LIT(__FUNCTION__), get_time_in_micros());
  while (parser->current < parser->tokens.len) {
    Ast_Decl *decl = parse_decl(parser, allocator);
    vector_append(&parser->file.decls, decl);
  }
  spall_buffer_end(&spall_ctx, &spall_buffer, get_time_in_micros());
}

internal void print_fields(Field_Vector fields) {
  vector_iter_v(fields, field, i, {
    if (i) {
      fmt_printc(", ");
    }
    fmt_printfc("%S: ", field->name);
    print_ast_type(field->type);
    if (field->value) {
      fmt_printc(" = ");
      print_ast_expr(field->value);
    }
  });
}

internal void print_ast_decl(Ast_Decl *d) {
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
        print_ast_stmt(stmt);
        fmt_printlnc(";");
      })
      fmt_printlnc("}");
      return;
    }
  case ANT_Decl_Type: {
      fmt_printfc("type %S ", d->decl_type->name);
      print_ast_type(d->decl_type->type);
      fmt_printlnc(";");
      return;
    }
  case ANT_Decl_Variable: {
      fmt_printfc("%S: ", d->decl_variable->name);
      print_ast_type(d->decl_variable->type);
      if (d->decl_variable->value) {
        if (d->decl_variable->constant) {
          fmt_printc(" : ");
        } else {
          fmt_printc(" = ");
        }
        print_ast_expr(d->decl_variable->value);
      }
      fmt_printlnc(";");
      return;
    }
  default:
    unreachable();
  }
}

internal void print_ast_stmt(Ast_Stmt *s) {
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
    print_ast_expr(s);
    return;

  case ANT_Decl_Import:
  case ANT_Decl_Function:
  case ANT_Decl_Type:
  case ANT_Decl_Variable:
    print_ast_decl(s);
    return;

  case ANT_Stmt_Defer: {
      fmt_printc("defer ");
      print_ast_stmt(s->stmt_defer->rhs);
      return;
    }
  case ANT_Stmt_Return: {
      fmt_printc("return ");
      slice_iter(s->stmt_return->values, r, i, {
        if (i) {
          fmt_printc(", ");
        }
        print_ast_expr(*r);
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
      print_ast_expr(s->stmt_if->cond);
      fmt_printlnc(" {");
      slice_iter_v(s->stmt_if->body, stmt, i, {
        fmt_printc("\t");
        print_ast_stmt(stmt);
        fmt_printlnc(";");
      });
      fmt_printc("}");
      return;
    }
  case ANT_Stmt_Loop: {
      fmt_printc("for [");
      if (s->stmt_loop->begin) {
        print_ast_expr(s->stmt_loop->begin);
        fmt_printc(", ");
      }
      print_ast_expr(s->stmt_loop->end);
      fmt_printc("] -> ");
      print_ast_decl(s->stmt_loop->decl);
      fmt_printlnc(" {");
      slice_iter_v(s->stmt_loop->body, stmt, i, {
        fmt_printc("\t");
        print_ast_stmt(stmt);
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
      print_ast_expr(s->stmt_iterator->lhs);
      fmt_printc(" -> ");
      slice_iter_v(s->stmt_iterator->decls, decl, i, {
         if (i) {
            fmt_printc(", ");
         }
        fmt_printfc("%S: ", decl->name);
        print_ast_type(decl->type);
      });
      fmt_printlnc(" {");
      slice_iter_v(s->stmt_iterator->body, stmt, i, {
        fmt_printc("\t");
        print_ast_stmt(stmt);
        fmt_printlnc(";");
      });
      fmt_printc("\t}");
      return;
    }
  case ANT_Stmt_Assign: {
      print_ast_expr(s->stmt_assign->lhs);
      fmt_printc(" = ");
      print_ast_expr(s->stmt_assign->rhs);

      return;
    }
  default:
    unreachable();
  }
}

internal void print_ast_expr(Ast_Expr *e) {
  fmt_printc("(");
  switch (e->ast_type) {
  case ANT_Expr_Selector: {
      print_ast_expr(e->expr_selector->lhs);
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
      print_ast_expr(e->expr_unary->rhs);
      break;
    }
  case ANT_Expr_Binary: {
      print_ast_expr(e->expr_binary->lhs);
      fmt_printfc(" %S ", binary_op_strings[e->expr_binary->op]);
      print_ast_expr(e->expr_binary->rhs);
      break;
    }
  case ANT_Expr_Ternary: {
      
      break;
    }
  case ANT_Expr_Call: {
      print_ast_expr(e->expr_call->lhs);
      fmt_printc("(");
      slice_iter_v(e->expr_call->args, arg, i, {
        if (i) {
          fmt_printc(", ");
        }
        print_ast_expr(arg);
      });
      fmt_printc(")");

      break;
    }
  case ANT_Expr_Deref: {
      print_ast_expr(e->expr_deref->lhs);
      fmt_printc("^");
      
      break;
    }
  case ANT_Expr_Cast: {
      fmt_printc("cast(");
      print_ast_type(e->expr_cast->type);
      fmt_printc(")");
      print_ast_expr(e->expr_cast->rhs);

      break;
    }
  case ANT_Expr_Index: {
      print_ast_expr(e->expr_index->base);
      fmt_printc("[");
      print_ast_expr(e->expr_index->index);
      fmt_printc("]");

      break;
    }
  default:
    unreachable();
  }
  fmt_printc(")");
}

internal void print_ast_type(Ast_Expr *t) {
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
    print_ast_type(t->type_pointer->pointee);
    break;
  }
  case ANT_Type_Array: {
    fmt_printc("[");
    if (t->type_array->count) {
      print_ast_expr(t->type_array->count);
    }
    fmt_printc("]");
    print_ast_type(t->type_array->elem);
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

#define TYPE_KINDS(X)                                                          \
  X(Type_Kind_Invalid )                                                        \
  X(Type_Kind_Array   )                                                        \
  X(Type_Kind_Slice   )                                                        \
  X(Type_Kind_Struct  )                                                        \
  X(Type_Kind_Union   )                                                        \
  X(Type_Kind_Enum    )                                                        \
  X(Type_Kind_Named   )                                                        \
  X(Type_Kind_Pointer )                                                        \
  X(Type_Kind_Basic   )                                                        \
  X(Type_Kind_Function)                                                        \
  X(Type_Kind_Untyped )                                                        \
  X(Type_Kind_Tuple   )

X_ENUM(Type_Kind, TYPE_KINDS);

#define BASIC_TYPES(X)                                                         \
  X(Basic_Type_Int    )                                                        \
  X(Basic_Type_Bool   )                                                        \
  X(Basic_Type_Uint   )                                                        \
  X(Basic_Type_Rune   )                                                        \
  X(Basic_Type_Float  )                                                        \
  X(Basic_Type_String )                                                        \
  X(Basic_Type_Cstring)                                                        \
  X(Basic_Type_Rawptr )                                                        \
  X(Basic_Type_Uintptr)

X_ENUM(Basic_Type_Kind, BASIC_TYPES);

#define UNTYPED_TYPES(X)                                                       \
  X(Untyped_Type_Int   )                                                       \
  X(Untyped_Type_Bool  )                                                       \
  X(Untyped_Type_Float )                                                       \
  X(Untyped_Type_String)                                                       \
  X(Untyped_Type_Rune  )

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
  Slice(Type *) fields;
} Type_Tuple;

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
  String name;
  Type  *type;
} Type_Named;

typedef struct {
  Basic_Type_Kind kind;
} Type_Basic;

typedef struct {
  Untyped_Type_Kind kind;
} Type_Untyped;

typedef struct {
  Slice(struct {
    String name;
    Type  *type;
  })       args;
  Type    *ret;
} Type_Function;

typedef struct {} Type_Invalid;

struct _Type {
  Type_Kind type_kind;
  isize     size, align;

  union {
    Type_Struct   struct_ [0];
    Type_Array    array   [0];
    Type_Slice    slice   [0];
    Type_Pointer  pointer [0];
    Type_Union    union_  [0];
    Type_Enum     enum_   [0];
    Type_Named    named   [0];
    Type_Function function[0];
    Type_Untyped  untyped [0];
    Type_Basic    basic   [0];
    Type_Invalid  invalid [0];

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
    (( Type *)type)[-1].align = (_align);                                      \
    (( Type *)type)[-1].size  = (_size);                                       \
    &((Type *)type)[-1];                                                       \
  })

typedef struct {
  Type *type;
  isize value;
} Constant;

typedef struct _Checker_Scope {
  Hash_Map(String, Type   *) types;
  Hash_Map(String, Type   *) variables;
  Hash_Map(String, Constant) constants;
  Type                      *return_value;
  struct _Checker_Scope     *parent;
} Checker_Scope;

internal void checker_scope_init(Checker_Scope *cs, Checker_Scope *parent, Allocator allocator) {
  cs->parent = parent;
  hash_map_init(&cs->variables, 16, string_equal, string_hash, allocator);
  hash_map_init(&cs->types    , 16, string_equal, string_hash, allocator);
  hash_map_init(&cs->constants, 16, string_equal, string_hash, allocator);
  cs->return_value = nil;
}

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

internal Constant *resolve_constant(Checker_Scope *scope, String name) {
  while (scope) {
    Constant *c = hash_map_get(scope->constants, name);
    if (c) {
      return c;
    }
    scope = scope->parent;
  }
  return nil;
}

internal Type *type_base_type(Type *type, Checker_Scope *cs) {
  if (!type) {
    return nil;
  }
  while (type->type_kind == Type_Kind_Named) {
    if (type->named->type) {
      type = type->named->type;
      continue;
    }
    type = resolve_named_type(cs, type->named->name);
  }
  return type;
}

internal b8 types_equal(Type *a, Type *b, Checker_Scope *cs) {
  if (!a || !b) {
    return false;
  }
  if (a->type_kind == Type_Kind_Invalid || b->type_kind == Type_Kind_Invalid) {
    return false;
  }
  if (a->type_kind == Type_Kind_Untyped) {
    Type *
    t = a;
    a = b;
    b = t;
  }
  if (b->type_kind == Type_Kind_Untyped) {
    a = type_base_type(a, cs);
    switch (a->type_kind) {
    case Type_Kind_Basic:
      switch (a->basic->kind) {
      case Basic_Type_Int:
      case Basic_Type_Uint:
        return b->untyped->kind == Untyped_Type_Int;
      case Basic_Type_Bool:
        return b->untyped->kind == Untyped_Type_Bool;
      case Basic_Type_Rune:
        return b->untyped->kind == Untyped_Type_Rune;
      case Basic_Type_Float:
        return b->untyped->kind == Untyped_Type_Float;
      case Basic_Type_String:
      case Basic_Type_Cstring:
        return b->untyped->kind == Untyped_Type_String;
      default:
        return false;
      }
    case Type_Kind_Untyped:
      return a->untyped->kind == b->untyped->kind;
    default:
      unreachable();
    }
  }
  if (a->type_kind != b->type_kind) {
    return false;
  }
  switch (a->type_kind) {
  case Type_Kind_Array:
    return (a->array->count == b->array->count) && types_equal(a->array->elem, b->array->elem, cs);
  case Type_Kind_Slice:
    return types_equal(a->slice->elem, b->slice->elem, cs);
  case Type_Kind_Struct:
    return false;
  case Type_Kind_Union:
    return false;
  case Type_Kind_Enum:
    return false;
  case Type_Kind_Named:
    return a == b;
  case Type_Kind_Pointer:
    return types_equal(a->pointer->pointee, a->pointer->pointee, cs);
  case Type_Kind_Basic:
    return mem_compare(a, b, size_of(Type) + size_of(Type_Basic));
  case Type_Kind_Function:
    return false;
  case Type_Kind_Untyped:
    return a->untyped->kind == b->untyped->kind;
  DEFAULT:
    unreachable();
  }
}

internal isize evaluate_constant_expression(Ast_Expr *e, Checker_Scope *scope) {
  switch (e->ast_type) {
  CASE ANT_Expr_Ident: {
    Constant *constant = resolve_constant(scope, e->expr_ident->name);
    if (constant) {
      return constant->value;
    }
    log_infof(LIT("Unresolved constant: %S"), e->expr_ident->name);
    return 0;
  }
  CASE ANT_Expr_Literal:
    switch (e->expr_literal->type) {
    CASE Token_Type_Int:
      return e->expr_literal->value.integer;
    CASE Token_Type_Float:
      return e->expr_literal->value.decimal;
    CASE Token_Type_Rune:
      return e->expr_literal->value.rune;
    CASE Token_Type_String:
      unimplemented();
    DEFAULT:
      unimplemented();
  }
  CASE ANT_Expr_Unary: {
    isize rhs = evaluate_constant_expression(e->expr_unary->rhs, scope);
    switch (e->expr_binary->op) {
    CASE Token_Type_Plus:
      return rhs;
    CASE Token_Type_Minus:
      return -rhs;
    CASE Token_Type_Not:
      return !rhs;
    DEFAULT:
      unimplemented();
    }
  }
  CASE ANT_Expr_Binary: {
    isize lhs = evaluate_constant_expression(e->expr_binary->lhs, scope);
    isize rhs = evaluate_constant_expression(e->expr_binary->rhs, scope);

    switch (e->expr_binary->op) {
    CASE Token_Type_Plus:
      return lhs + rhs;
    CASE Token_Type_Minus:
      return lhs - rhs;
    CASE Token_Type_Multiply:
      return lhs * rhs;
    CASE Token_Type_Divide:
      return lhs / rhs;
    CASE Token_Type_And:
      return lhs && rhs;
    CASE Token_Type_Or:
      return lhs || rhs;
    CASE Token_Type_Bit_And:
      return lhs & rhs;
    CASE Token_Type_Bit_Or:
      return lhs | rhs;
    CASE Token_Type_Less:
      return lhs < rhs;
    CASE Token_Type_More:
      return lhs > rhs;
    CASE Token_Type_Equal:
      return lhs == rhs;
    CASE Token_Type_Less_Equal:
      return lhs <= rhs;
    CASE Token_Type_More_Equal:
      return lhs >= rhs;
    CASE Token_Type_Modulo:
      return lhs % rhs;
    DEFAULT:
      unimplemented();
    }
  }
  CASE ANT_Expr_Ternary:
    unimplemented();
  CASE ANT_Expr_Call:
    unimplemented();
  CASE ANT_Expr_Selector:
    unimplemented();
  CASE ANT_Expr_Deref:
    unimplemented();
  CASE ANT_Expr_Cast:
    unimplemented();
  CASE ANT_Expr_Index:
    unimplemented();
  CASE ANT_Expr_Address:
    unimplemented();
  DEFAULT:
    unimplemented();
  }

  unreachable();
}

internal Type *resolve_ast_type(Ast_Expr *t, Checker_Scope *scope, Allocator allocator) {
  SWITCH (t->ast_type) {
  CASE ANT_Type_Function: {
    Type_Function *f = type_new(Function, allocator);
    Ast_Type_Function *function = t->type_function;
    slice_init(&f->args, function->args.len, allocator);

    slice_iter_v(function->args, arg, i, {
      f->args.data[i].type = resolve_ast_type(arg->type, scope, allocator);
      f->args.data[i].name = arg->name;
    });
    if (function->returns.len > 1) {
      Type_Tuple *tuple = type_new(Tuple, allocator);
      slice_init(&tuple->fields, function->returns.len, allocator);
      slice_iter_v(function->returns, ret, i, {
        tuple->fields.data[i] = resolve_ast_type(ret->type, scope, allocator);
      });
      f->ret = type_base(tuple, 0, 0);
    } else if (function->returns.len) {
      f->ret = resolve_ast_type(function->returns.data[0]->type, scope, allocator);
    }
    return type_base(f, 8, 8);
  }
  CASE ANT_Type_Pointer: {
    Type_Pointer *p = type_new(Pointer, allocator);
    p->pointee = resolve_ast_type(t->type_pointer->pointee, scope, allocator);
    return type_base(p, 8, 8);
  }
  CASE ANT_Type_Array: {
    Type_Array *a = type_new(Array, allocator);
    a->elem = resolve_ast_type(t->type_array->elem, scope, allocator);
    if (t->type_array->count) {
      a->count = evaluate_constant_expression(t->type_array->count, scope);
      return type_base(a, a->elem->size * a->count, a->elem->align);
    } else {
      a->count = nil;
      return type_base(a, 16, 8);
    }
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
  CASE ANT_Type_Named:
    return resolve_named_type(scope, t->type_named->name);
  DEFAULT:
    unreachable();
  }
  unreachable();
}

internal Type *new_basic_type(Basic_Type_Kind kind, isize size, isize align, Allocator allocator) {
  Type_Basic *b = type_new(Basic, allocator);
  b->kind = kind;
  return type_base(b, size, align);
}

internal Type *new_untyped_type(Untyped_Type_Kind kind, Allocator allocator) {
  Type_Untyped *b = type_new(Untyped, allocator);
  b->kind = kind;
  return type_base(b, 0, 0);
}

Type *type_untyped_bool   = nil;
Type *type_untyped_int    = nil;
Type *type_untyped_float  = nil;
Type *type_untyped_string = nil;

Type *type_invalid        = nil;

internal Type          *type_check_expr(Ast_Expr *expr, Checker_Scope *cs, Allocator allocator);
internal Type_Function *type_check_decl_function(Ast_Decl_Function *function, Checker_Scope *cs, Allocator allocator);
internal void           type_check_decl_variable(Ast_Decl_Variable *variable, Checker_Scope *cs, Allocator allocator);
internal void           type_check_scope(Ast_Stmt_Block body, Checker_Scope *cs, Allocator allocator);


internal void type_check_decl_type(Ast_Decl_Type *type_decl, Checker_Scope *cs, Allocator allocator) {
  Type *t = resolve_ast_type(type_decl->type, cs, allocator);
  hash_map_insert(&cs->types, type_decl->name, t);
}

internal void print_type(Type *type);

internal void type_check_stmt_assign(Ast_Stmt_Assign *assign, Checker_Scope *cs, Allocator allocator) {
  Type *lhs = type_check_expr(assign->lhs, cs, allocator);
  Type *rhs = type_check_expr(assign->rhs, cs, allocator);

  // print_type(lhs);
  // fmt_printc(" == ");
  // print_type(rhs);
  // fmt_printlnc("");

  // if (lhs) { log_infof(LIT("lhs: %S(%x) at %d"), enum_to_string(Type_Kind, lhs->type_kind), lhs, assign->lhs->location.line); }
  // if (rhs) { log_infof(LIT("rhs: %S(%x) at %d"), enum_to_string(Type_Kind, rhs->type_kind), rhs, assign->rhs->location.line); }
  // if (lhs) {
  //   log_infof(LIT("lhs == rhs -> %B"), lhs == rhs);
  // }
}

internal b8 type_is_boolean(Type *type, Checker_Scope *cs) {
  type = type_base_type(type, cs);
  if (!type) {
    return false;
  }
  return (type->type_kind == Type_Kind_Basic   && type->basic->kind   == Basic_Type_Bool  ) ||
         (type->type_kind == Type_Kind_Untyped && type->untyped->kind == Untyped_Type_Bool);
}

internal b8 type_is_integer(Type *type, Checker_Scope *cs) {
  type = type_base_type(type, cs);
  return (type->type_kind == Type_Kind_Basic) && (type->basic->kind == Basic_Type_Int || type->basic->kind == Basic_Type_Uint);
}

internal void type_check_stmt(Ast_Stmt *s, Checker_Scope *cs, Allocator allocator) {
  switch (s->ast_type) {
  CASE ANT_Decl_Function:
    type_check_decl_function(s->decl_function, cs, allocator);
  CASE ANT_Decl_Type:
    type_check_decl_type(s->decl_type, cs, allocator);
  CASE ANT_Decl_Variable:
    type_check_decl_variable(s->decl_variable, cs, allocator);
  CASE ANT_Stmt_Defer:
    type_check_stmt(s->stmt_defer->rhs, cs, allocator);
  CASE ANT_Stmt_Return:
  CASE ANT_Stmt_Break:
  CASE ANT_Stmt_Continue:
  CASE ANT_Stmt_Block: {
    Checker_Scope scope;
    checker_scope_init(&scope, cs, allocator);
    type_check_scope(*s->stmt_block, &scope, allocator);
  }
  CASE ANT_Stmt_If: {
    Checker_Scope scope;
    checker_scope_init(&scope, cs, allocator);
    Type *cond_type = type_check_expr(s->stmt_if->cond, cs, allocator);
    if (!type_is_boolean(cond_type, cs)) {
      error(Error_Type_Type, s->stmt_if->cond->location, LIT("Expected a boolean expression for if statement condition"));
    }
    type_check_scope(s->stmt_if->body, &scope, allocator);
  }
  CASE ANT_Stmt_Loop: {
    Checker_Scope scope;
    checker_scope_init(&scope, cs, allocator);
    type_check_scope(s->stmt_loop->body, &scope, allocator);
  }
  CASE ANT_Stmt_Iterator: {
    
  }
  CASE ANT_Stmt_Switch:
  CASE ANT_Stmt_Assign:
    type_check_stmt_assign(s->stmt_assign, cs, allocator);
  DEFAULT: {}
  }
}

internal void type_check_scope(Ast_Stmt_Block body, Checker_Scope *cs, Allocator allocator) {
  slice_iter_v(body, stmt, _i, {
    type_check_stmt(stmt, cs, allocator);
  });
  fmt_printlnc("{");
  hash_map_iter(cs->types, name, type, {
    fmt_printflnc("\tType: %12S, Size: %2d, Align: %2d", name, (*type)->size, (*type)->align);
  });
  hash_map_iter(cs->variables, name, type, {
    fmt_printflnc("\tVar:  %12S, Size: %2d, Align: %2d", name, (*type)->size, (*type)->align);
  });
  fmt_printlnc("}");
}

internal Type_Function *type_check_decl_function(Ast_Decl_Function *function, Checker_Scope *cs, Allocator allocator) {
  Type_Function *f = type_new(Function, allocator);
  slice_init(&f->args, function->args.len, allocator);

  Checker_Scope scope;
  checker_scope_init(&scope, cs, allocator);

  slice_iter_v(function->args, arg, i, {
    f->args.data[i].type = resolve_ast_type(arg->type, cs, allocator);
    f->args.data[i].name = arg->name;

    hash_map_insert(&scope.variables, f->args.data[i].name, f->args.data[i].type);
  });
  if (function->returns.len > 1) {
    Type_Tuple *tuple = type_new(Tuple, allocator);
    slice_init(&tuple->fields, function->returns.len, allocator);

    slice_iter_v(function->returns, ret, i, {
      tuple->fields.data[i] = resolve_ast_type(ret->type, cs, allocator);
      hash_map_insert(&scope.variables, ret->name, tuple->fields.data[i]);
    });
    f->ret = type_base(tuple, 0, 0);
  } else if (function->returns.len) {
    f->ret = resolve_ast_type(function->returns.data[0]->type, cs, allocator);
    hash_map_insert(&scope.variables, function->returns.data[0]->name, f->ret);
  }

  scope.return_value = f->ret;

  type_check_scope(function->body, &scope, allocator);

  return f;
}

internal void print_type(Type *type) {
  if (type == nil) {
    fmt_printc("<nil>");
    return;
  }
  switch (type->type_kind) {
  CASE Type_Kind_Array: {
    fmt_printfc("[%d]", type->array->count);
    print_type(type->array->elem);
  }
  CASE Type_Kind_Struct: {
    
  }
  CASE Type_Kind_Union: {
    
  }
  CASE Type_Kind_Enum: {
    
  }
  CASE Type_Kind_Named: {
    fmt_printfc("%S (aka ", type->named->name);
    print_type(type->named->type);
    fmt_printc(")");
  }
  CASE Type_Kind_Pointer: {
    fmt_printc("^");
    print_type(type->pointer->pointee);
  }
  CASE Type_Kind_Basic: {
    fmt_printfc("%S", enum_to_string(Basic_Type_Kind, type->basic->kind));
  }
  CASE Type_Kind_Function: {
    
  }
  CASE Type_Kind_Untyped: {
    
  }
  CASE Type_Kind_Tuple: {
    
  }
  DEFAULT:
    unreachable();
  }
}

internal void type_check_decl_variable(Ast_Decl_Variable *variable, Checker_Scope *cs, Allocator allocator) {
  Type *type = resolve_ast_type(variable->type, cs, allocator);
  if (variable->constant) {
    Constant c;
    c.value = evaluate_constant_expression(variable->value, cs);
    c.type  = type;
    hash_map_insert(&cs->constants, variable->name, c);
    // log_infof(LIT("Constant %S: %S (size: %d, align: %d)"), variable->name, enum_to_string(Type_Kind, type->type_kind), type->size, type->align);
    return;
  }
  if (variable->value) {
    type_check_expr(variable->value, cs, allocator);
  }
  // log_infof(LIT("Variable %S: %S (size: %d, align: %d)"), variable->name, enum_to_string(Type_Kind, type->type_kind), type->size, type->align);
  hash_map_insert(&cs->variables, variable->name, type);
}

internal Type *type_check_expr(Ast_Expr *expr, Checker_Scope *cs, Allocator allocator) {
  switch (expr->ast_type) {
  CASE ANT_Expr_Ident:
    return resolve_ident(cs, expr->expr_ident->name);
  CASE ANT_Expr_Literal: {
    Type_Untyped *u = type_new(Untyped, allocator);
    switch (expr->expr_literal->type) {
    CASE Token_Type_String:
      u->kind = Untyped_Type_String;
    CASE Token_Type_Int:
      u->kind = Untyped_Type_Int;
    CASE Token_Type_Float:
      u->kind = Untyped_Type_Float;
    CASE Token_Type_Rune:
      u->kind = Untyped_Type_Rune;
    CASE Token_Type_True:
      u->kind = Untyped_Type_Bool;
    CASE Token_Type_False:
      u->kind = Untyped_Type_Bool;
    DEFAULT:
      unreachable();
    }
    return type_base(u, 0, 0);
  }
  CASE ANT_Expr_Unary:
    return type_check_expr(expr->expr_unary->rhs, cs, allocator);
  CASE ANT_Expr_Binary: {
    Type *lhs = type_check_expr(expr->expr_binary->lhs, cs, allocator);
    Type *rhs = type_check_expr(expr->expr_binary->rhs, cs, allocator);

    if (!types_equal(lhs, rhs, cs)) {
      error(Error_Type_Type, expr->location, LIT("Operands for binary operation have to of the same type: "));
      print_type(lhs);
      fmt_printc(" != ");
      print_type(rhs);
      fmt_printlnc(";");
    }

    switch (expr->expr_binary->op) {
    CASE Token_Type_Plus:
      return lhs;
    CASE Token_Type_Minus:
      return lhs;
    CASE Token_Type_Multiply:
      return lhs;
    CASE Token_Type_Divide:
      return lhs;
    CASE Token_Type_Not:
      return lhs;
    CASE Token_Type_And:
      return lhs;
    CASE Token_Type_Or:
      return lhs;
    CASE Token_Type_Bit_And:
      return lhs;
    CASE Token_Type_Bit_Or:
      return lhs;
    CASE Token_Type_Modulo:
      return lhs;
    CASE Token_Type_Less:
      return type_untyped_bool;
    CASE Token_Type_More:
      return type_untyped_bool;
    CASE Token_Type_Equal:
      return type_untyped_bool;
    CASE Token_Type_Less_Equal:
      return type_untyped_bool;
    CASE Token_Type_More_Equal:
      return type_untyped_bool;
    DEFAULT:
      return type_invalid;
    }
  }
  CASE ANT_Expr_Ternary:
  CASE ANT_Expr_Call: {
    Type *t = type_check_expr(expr->expr_call->lhs, cs, allocator);
    if (t->type_kind != Type_Kind_Function) {
      panic("call operator can only be used on functions");
    }
  }
  CASE ANT_Expr_Selector: {
    // Type *t = type_check_expr(expr->expr_selector->lhs, cs, allocator);
    // if (t->type_kind != Type_Kind_Struct) {
    //   panic("selector operator can only be used on structs");
    // }
  }
  CASE ANT_Expr_Deref: {
    Type *lhs = type_check_expr(expr->expr_deref->lhs, cs, allocator);
    lhs = type_base_type(lhs, cs);
    if (lhs->type_kind != Type_Kind_Pointer) {
      errorc(Error_Type_Type, expr->expr_deref->lhs->location, "Only pointer types can be dereferenced");
      return type_invalid;
    }
    return lhs->pointer->pointee;
  }
  CASE ANT_Expr_Cast:
  CASE ANT_Expr_Index: {
    Type *lhs = type_check_expr(expr->expr_index->base,  cs, allocator);
    Type *rhs = type_check_expr(expr->expr_index->index, cs, allocator);
    lhs = type_base_type(lhs, cs);
    if (lhs->type_kind == Type_Kind_Array) {
      return lhs->array->elem;
    }
    return nil;
  }
  CASE ANT_Expr_Address: {
    Type_Pointer *p = type_new(Pointer, allocator);
    p->pointee      = type_check_expr(expr->expr_address->rhs, cs, allocator);
    return type_base(p, 8, 8);
  }
  DEFAULT: {}
  }
  return nil;
}

internal b8 type_check_file(Ast_File file, Allocator allocator) {
  spall_buffer_begin(&spall_ctx, &spall_buffer, LIT(__FUNCTION__), get_time_in_micros());
  b8 ok = true;

  Checker_Scope cs;
  checker_scope_init(&cs, nil, allocator);

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

  type_untyped_int = new_untyped_type(Untyped_Type_Int, allocator);
  type_untyped_bool = new_untyped_type(Untyped_Type_Bool, allocator);
  type_untyped_float = new_untyped_type(Untyped_Type_Float, allocator);
  type_untyped_string = new_untyped_type(Untyped_Type_String, allocator);

  type_invalid = type_base(type_new(Invalid, allocator), 0, 0);
  
  #undef BASIC_TYPE
  
  slice_iter_v(file.decls, decl, _i, {
    switch (decl->ast_type) {
    CASE ANT_Decl_Type: {
      type_check_decl_type(decl->decl_type, &cs, allocator);
    }
    CASE ANT_Decl_Variable: {
      type_check_decl_variable(decl->decl_variable, &cs, allocator);
    }
    CASE ANT_Decl_Function: {
      type_check_decl_function(decl->decl_function, &cs, allocator);
    }
    DEFAULT: {}
    }
  });

  // hash_map_iter(cs.types, name, type, {
  //   fmt_printflnc("Type: %12S, Size: %2d, Align: %2d", name, (*type)->size, (*type)->align);
  // });

  // hash_map_iter(cs.variables, name, type, {
  //   fmt_printflnc("Var:  %12S, Size: %2d, Align: %2d", name, (*type)->size, (*type)->align);
  // });

  spall_buffer_end(&spall_ctx, &spall_buffer, get_time_in_micros());
  return ok;
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

b8 spall_write_callback(SpallProfile *self, const void *data, isize length) {
  return file_write((Fd)self->data, (Byte_Slice) {.data = (byte *)data, .len = length}).err == OSE_None;
}

void spall_close_callback(SpallProfile *self) {
  file_close((Fd)self->data);
}

i32 main() {
  context.allocator = panic_allocator();
  context.logger    = create_file_logger(FD_STDERR);

  Fd spall_fd = unwrap_err(file_open(LIT("trace.spall"), FP_Create | FP_Read_Write | FP_Truncate));
  spall_ctx   = spall_init_callbacks(1, spall_write_callback, nil, spall_close_callback, (rawptr)spall_fd);
	Byte_Slice spall_buffer_backing = slice_make(Byte_Slice, 1024 * 4, context.temp_allocator);
	spall_buffer = (SpallBuffer){
		.length = spall_buffer_backing.len,
		.data   = spall_buffer_backing.data,
	};
	spall_buffer_init(&spall_ctx, &spall_buffer);

  // context.temp_allocator.proc = printing_allocator_proc;

  if (os_args.len < 2) {
    fmt_eprintflnc("Usage: %S <file> <flags>", os_args.data[0]);
    os_exit(1);
  }

  isize allocated = 0;

  slice_iter_v(slice_start(os_args, 1), path, _i, {
    spall_buffer_begin(&spall_ctx, &spall_buffer, LIT("read_entire_file_path"), get_time_in_micros());
    Byte_Slice file_data = or_do_err(read_entire_file_path(path, context.temp_allocator), err, {
      fmt_eprintflnc("Failed to read file '%S': '%S'", path, enum_to_string(OS_Error, err));
      continue;
    });
    spall_buffer_end(&spall_ctx, &spall_buffer, get_time_in_micros());
    Token_Vector tokens = vector_make(Token_Vector, 0, 8, context.temp_allocator);
    if (!tokenize_file(bytes_to_string(file_data), path, &tokens)) {
      fmt_eprintflnc("Failed to tokenize file '%S'", path);
      continue;
    }

    Parser parser    = {0};
    parser.tokens    = vector_to_slice(type_of(parser.tokens), tokens);
    parser.file.path = path;
    vector_init(&parser.file.decls, 0, 8, context.temp_allocator);

    parse_file(&parser, context.temp_allocator);

    if (!type_check_file(parser.file, context.temp_allocator)) {
      fmt_eprintflnc("Type checking failed for file: '%S'", path);
      continue;
    }
  });

	spall_buffer_quit(&spall_ctx, &spall_buffer);
	spall_quit(&spall_ctx);

  return 0;
}
