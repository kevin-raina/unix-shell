#pragma once

#include "../include/shell.h"
#include <ctype.h>

typedef enum {
  TOKEN_WORD,       // ls, gret, foo.txt
  TOKEN_PIPE,       // |
  TOKEN_REDIR_IN,   // <
  TOKEN_REDIR_OUT,  // >
  TOKEN_APPEND,     // >>
  TOKEN_BACKGROUND, // &
  TOKEN_EOF,
} TokenType;

typedef struct {
  TokenType type;
  char *value;
} Token;

Token *sh_tokenize(char *line);
