#include "../include/lexer.h"

char *endofline(char *p) {
  char *q = 0;
  int i = 0;
  while (isalpha(p[i])) {
    i++;
    q++;
  }
  return q;
}

int isassignment(char *p) {
  char *q = endofline(p);
  if (q == p)
    return 0;
  return *q == '=';
}

token *tokenize(char *line) {
  token *tokens = malloc(64 * sizeof(token));
  int count = 0;
  int i = 0;

  while (line[i] != '\0') {
    if (isspace(line[i])) {
      i++;
      continue;
    }
    if (line[i] == '(') {
      tokens[count].type = LPAREN;
      tokens[count].value = NULL;
      count++;
      i++;
    }

    else if (line[i] == '|') {
      tokens[count].type = PIPE;
      tokens[count].value = NULL;
      count++;
      i++;
    } else if (line[i] == ')') {
      tokens[count].type = RPAREN;
      tokens[count].value = NULL;
      count++;
      i++;
    }

    else if (line[i] == '{') {
      tokens[count].type = LBRACE;
      tokens[count].value = NULL;
      count++;
      i++;
    }

    else if (line[i] == '}') {
      tokens[count].type = RBRACE;
      tokens[count].value = NULL;
      count++;
      i++;
    }

    else if (line[i] == ';') {
      tokens[count].type = SEMI;
      tokens[count].value = NULL;
      count++;
      i++;
    }

    else if (line[i] == '&') {
      tokens[count].type = AMP;
      tokens[count].value = NULL;
      count++;
      i++;
    }

    else if (line[i] == '\n') {
      tokens[count].type = NEWLINE;
      tokens[count].value = NULL;
      count++;
      i++;
    }

    else if (line[i] == ';' && line[i + 1] == ';') {
      tokens[count].type = DSEMI;
      tokens[count].value = NULL;
      count++;
      i++;
    }

    else if (line[i] == '&' && line[i + 1] == '&') {
      tokens[count].type = LOGAND;
      tokens[count].value = NULL;
      count++;
      i++;
    }

    else if (line[i] == '|' && line[i + 1] == '|') {
      tokens[count].type = LOGOR;
      tokens[count].value = NULL;
      count++;
      i++;
    }

    else if (line[i] == '<') {
      tokens[count].type = REDIR_IN;
      tokens[count].value = NULL;
      count++;
      i++;
    }

    else if (line[i] == '>') {
      tokens[count].type = REDIR_OUT;
      tokens[count].value = NULL;
      count++;
      i++;
    }

    else if (line[i] == '>' && line[i + 1] == '>') {
      tokens[count].type = REDIR_APPEND;
      tokens[count].value = NULL;
      count++;
      i += 2;
    }

    else if (line[i] == '<' && line[i + 1] == '<') {
      tokens[count].type = REDIR_HEREDOC;
      tokens[count].value = NULL;
      count++;
      i += 2;
    }

    else if (line[i] == '<' && line[i + 1] == '&') {
      tokens[count].type = REDIR_DUP_IN;
      tokens[count].value = NULL;
      count++;
      i += 2;
    }

    else if (line[i] == '>' && line[i + 1] == '&') {
      tokens[count].type = REDIR_DUP_OUT;
      tokens[count].value = NULL;
      count++;
      i += 2;

    }

    else if (line[i] == '<' && line[i + 1] == '>') {
      tokens[count].type = REDIR_RDWR;
      tokens[count].value = NULL;
      count++;
      i += 2;
    }

    else if (line[i] == '>' && line[i + 1] == '|') {
      tokens[count].type = REDIR_CLOBBER;
      tokens[count].value = NULL;
      count++;
      i += 2;
    }

    else if (isalpha(line[i])) {
      int start = i;
      while (line[i] != '\0' && !isspace(line[i]) && line[i] != '>' &&
             line[i] != '<' && line[i] != '|' && line[i] != '&') {
        i++;
      }
      int len = i - start;
      if (isassignment(line)) {
        tokens[count].type = ASSIGNMENT_WORD;
      } else {
        tokens[count].type = WORD;
      }
      tokens[count].value = malloc(len + 1);
      strncpy(tokens[count].value, line + start, len);
      tokens[count].value[len] = '\0';
      count++;
      continue;
    }

    else if (isdigit(line[i]) && line[i + 1] == '>') {
      tokens[count].type = IO_NUMBER;
      tokens[count].value = NULL;
      count++;
      i++;
    }
  }
  tokens[count].type = EOF;
  tokens[count].value = NULL;
  return tokens;
}
