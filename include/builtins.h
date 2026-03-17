#pragma once

#include "../include/jobs.h"
#include <stdio.h>
#include <stdlib.h>

extern char *builtin_str[];
extern int (*builtin_func[])(char **);
extern int sh_num_builtins();

//  function declarations for builtin shell commands
int sh_cd(char **args);
int sh_help(char **args);
int sh_exit(char **args);
int sh_jobs(char **args);
int sh_fg(char **args);
int sh_bg(char **args);
