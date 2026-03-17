#include "../include/parser.h"

Command sh_parse(Token *tokens) {
  Command command;
  command.args = malloc(64 * sizeof(char *));
  command.redirect_in = NULL;
  command.redirect_out = NULL;
  command.append = NULL;
  command.background = 0;

  int arg_count = 0;
  int i = 0;

  while (tokens[i].type != TOKEN_EOF) {
    if (tokens[i].type == TOKEN_WORD) {
      command.args[arg_count] = tokens[i].value;
      arg_count++;
    }

    if (tokens[i].type == TOKEN_REDIR_IN) {
      i++;
      if (tokens[i].type != TOKEN_WORD) {
        fprintf(stderr, "parse error near '>'\n");
      }
      command.redirect_in = tokens[i].value;
    }

    if (tokens[i].type == TOKEN_REDIR_OUT) {
      i++;
      if (tokens[i].type != TOKEN_WORD) {
        fprintf(stderr, "parse error near '<'\n");
      }
      command.redirect_out = tokens[i].value;
    }

    if (tokens[i].type == TOKEN_APPEND) {
      i++;
      if (tokens[i].type != TOKEN_WORD) {
        fprintf(stderr, "parse error near '<'\n");
      }
      command.append = tokens[i].value;
    }

    if (tokens[i].type == TOKEN_BACKGROUND) {
      command.background = 1;
    }
    i++;
  }

  command.args[arg_count] = NULL;
  return command;
}
