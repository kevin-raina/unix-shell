#pragma once

#include "../include/builtins.h"

char *sh_read_line(void);
char **sh_split_line(char *line);
int sh_launch(char **args);
int sh_execute(char **args);
void sh_loop(void);
