#include "../include/parser.h"
#include <stdio.h>

int main() {
  Token *tokens = sh_tokenize("ls -la > out.txt &");
  Command cmd = sh_parse(tokens);

  printf("args: ");
  for (int i = 0; cmd.args[i] != NULL; i++)
    printf("%s ", cmd.args[i]);
  printf("\n");
  printf("redirect_out: %s\n", cmd.redirect_out ? cmd.redirect_out : "NULL");
  printf("background: %d\n", cmd.background);
  return 0;
}
