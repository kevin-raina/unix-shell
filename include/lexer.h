#pragma once

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
  // words and literals
  WORD,
  ASSIGNMENT_WORD,
  IO_NUMBER,

  // operators
  PIPE,    // |
  LPAREN,  // (
  RPAREN,  // )
  LBRACE,  // {
  RBRACE,  // }
  SEMI,    // ;
  AMP,     // &
  NEWLINE, // '\n'
  DSEMI,   // ;;
  LOGAND,  // &&
  LOGOR,   // ||

  // redirections
  REDIR_IN,      // <
  REDIR_OUT,     // >
  REDIR_APPEND,  // >>
  REDIR_HEREDOC, // <<
  REDIR_DUP_IN,  // <&
  REDIR_DUP_OUT, // >&
  REDIR_RDWR,    // <>
  REDIR_CLOBBER, // >|

  // keywords
  IF,
  THEN,
  ELSE,
  ELIF,
  FI,
  WHILE,
  DO,
  DONE,
  FOR,
  IN,
  CASE,
  ESAC,
  UNTIL,
  BANG,

  // special
  END,

} tokentype;

typedef struct {
  tokentype type;
  char *value;
} token;

char *endofline(char *p);
int isassignment(char *p);
token *tokenize(char *line);
