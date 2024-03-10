#ifndef KEVCC_KLANG_INCLUDE_PARSE_KLTOKENS_H
#define KEVCC_KLANG_INCLUDE_PARSE_KLTOKENS_H

typedef enum tagKlTokenKind {
  KLTK_ERR = -1,
  KLTK_ID = 0,
  KLTK_INT,
  KLTK_STRING,
  KLTK_BOOLVAL,
  KLTK_NIL,
  /* operators */
  KLTK_CONCAT,
  KLTK_ADD,
  KLTK_MINUS,
  KLTK_MUL,
  KLTK_DIV,
  KLTK_MOD,
  /* compare */
  KLTK_LT,
  KLTK_LE,
  KLTK_GT,
  KLTK_GE,
  KLTK_EQ,
  KLTK_NE,
  /* boolean */
  KLTK_AND,
  KLTK_OR,
  KLTK_NOT,


  KLTK_LPAREN,
  KLTK_RPAREN,
  KLTK_LBRACKET,
  KLTK_RBRACKET,
  KLTK_LBRACE,
  KLTK_RBRACE,

  KLTK_DOT,
  KLTK_COMMA,
  KLTK_SEMI,
  KLTK_COLON,
  KLTK_QUESTION,
  KLTK_ARROW,

  KLTK_ASSIGN,
  KLTK_BAR,

  /* keywords */
  KLTK_IF,
  KLTK_ELSE,
  KLTK_WHILE,
  KLTK_REPEAT,
  KLTK_UNTIL,
  KLTK_FOR,
  KLTK_IN,
  KLTK_LET,
  KLTK_CLASS,
  KLTK_RETURN,
  KLTK_BREAK,
  KLTK_CONTINUE,
  KLTK_LOCAL,
  KLTK_SHARED,
  KLTK_NEW,

  KLTK_END,
  KLTK_NTOKEN,
} KlTokenKind;

#define kltoken_isbinop(kind)   ((kind) >= KLTK_CONCAT && (kind) <= KLTK_OR)

const char* kltoken_desc(KlTokenKind kind);


#endif
