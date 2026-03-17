#pragma once

#include "../include/lexer.h"

typedef struct {
  char **args;        // command and it's arguments
  char *redirect_in;  // filename for < or NULL
  char *redirect_out; // filename for > or NULL
  char *append;       // filename for >> or NULL
  int background;     // 1 if & present
} Command;

Command sh_parse(Token *tokens);
