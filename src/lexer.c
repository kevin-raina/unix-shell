#include "../include/lexer.h"

Token *sh_tokenize(char *line) {
  Token *tokens = malloc(64 * sizeof(Token));
  int count = 0;
  int i = 0;

  while (line[i] != '\0') {
    // skip whitespace
    if (isspace(line[i])) {
      i++;
      continue;
    }

    // check for >>
    if (line[i] == '>' && line[i + 1] == '>') {
      tokens[count].type = TOKEN_APPEND;
      tokens[count].value = NULL;
      count++;
      i += 2;
    }
    // check for >
    else if (line[i] == '>') {
      tokens[count].type = TOKEN_REDIR_OUT;
      tokens[count].value = NULL;
      count++;
      i++;
    }
    // check for <
    else if (line[i] == '<') {
      tokens[count].type = TOKEN_REDIR_IN;
      tokens[count].value = NULL;
      count++;
      i++;

    }
    // check for |
    else if (line[i] == '|') {
      tokens[count].type = TOKEN_PIPE;
      tokens[count].value = NULL;
      count++;
      i++;

    }
    // check for &
    else if (line[i] == '&') {
      tokens[count].type = TOKEN_BACKGROUND;
      tokens[count].value = NULL;
      count++;
      i++;
    }
    // otherwise it's a word
    else {
      // read characters until whitespace or operator
      int start = i;
      while (line[i] != '\0' && !isspace(line[i]) && line[i] != '>' &&
             line[i] != '<' && line[i] != '|' && line[i] != '&') {
        i++;
      }
      // copy word into token
      int len = i - start;
      tokens[count].type = TOKEN_WORD;
      tokens[count].value = malloc(len + 1);
      strncpy(tokens[count].value, line + start, len);
      tokens[count].value[len] = '\0';
      count++;
      continue;
    }
  }

  // add EOF token
  tokens[count].type = TOKEN_EOF;
  tokens[count].value = NULL;
  return tokens;
}
