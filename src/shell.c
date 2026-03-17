#include "../include/parser.h"

#include <stdio.h>
#include <stdlib.h>

#define SH_RL_BUFSIZE 1024
#define SH_TOK_BUFSIZE 64
#define SH_TOK_DELIM " \t\r\n\a"

char *sh_read_line(void) {
  int bufsize = SH_RL_BUFSIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;

  // if memory allocation fails
  if (!buffer) {
    fprintf(stderr, "sh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  while (1) {
    // read a charcter
    c = getchar();

    // if we hit EOF, replace it with a null character and return
    if (c == '\n') {
      buffer[position] = '\0';
      return buffer;
    }

    else if (c == EOF) {
      if (feof(stdin)) {
        // Ctrl+D, real end of file, exit
        exit(EXIT_SUCCESS);
      } else {
        // Ctrl+C interrupted getchar(), reset and continue
        clearerr(stdin);
        buffer[position] = '\0';
        return buffer;
      }
    }

    else {
      buffer[position] = c;
    }
    position++;

    // if we have exceeded the buffer, rellocate
    if (position >= bufsize) {
      bufsize += SH_RL_BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer) {
        fprintf(stderr, "sh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}

char **sh_split_line(char *line) {
  int bufsize = SH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char *));
  char *token;

  if (!tokens) {
    fprintf(stderr, "sh:allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, SH_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += SH_TOK_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char *));
      if (!tokens) {
        fprintf(stderr, "sh:allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
    token = strtok(NULL, SH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

int sh_launch(char **args) {
  // ignore SIGTTOU signal
  signal(SIGTTOU, SIG_IGN);

  pid_t pid;
  int status;

  int last = 0;
  while (args[last + 1] != NULL)
    last++;

  int background = 0;

  if (strcmp(args[last], "&") == 0) {
    background = 1;
    args[last] = NULL;
  } else {
    background = 0;
  }

  pid = fork();
  if (pid == 0) {
    // child process
    // child becomes its own process group
    setpgid(0, 0);
    // restore default behavior
    signal(SIGINT, SIG_DFL);
    signal(SIGTSTP, SIG_DFL);

    if (execvp(args[0], args) == -1) {
      perror("sh");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // error forking
    perror("sh");
  } else {
    // parent also set's child's group to avoid race conditions
    setpgid(pid, pid);
    // parent process
    if (background == 0) {
      // foreground: Wait for child to finish
      // give terminal to child
      tcsetpgrp(STDIN_FILENO, pid);

      do {
        waitpid(pid, &status, WUNTRACED);
      } while (!WIFEXITED(status) && !WIFSIGNALED(status) &&
               !WIFSTOPPED(status));

      if (WIFSTOPPED(status)) {
        // job was stopped with Ctrl+Z
        job_table[job_count].pid = pid;
        // store full command string
        store_command(args);
        job_table[job_count].status = STOPPED;
        job_count++;
        printf("[%d] %d suspended %s\n", job_count, pid, args[0]);
      }

      // take terminal back
      tcsetpgrp(STDIN_FILENO, shell_pgid);
    }

    else {
      // background: Do not wait, print pid and return
      job_table[job_count].pid = pid;
      store_command(args);
      job_table[job_count].status = RUNNING;
      job_count++;
      printf("[%d] %d\n", job_count, pid);
    }
  }
  return 1;
}

int sh_execute(char **args) {
  if (args[0] == NULL) {
    // an empty command was entered
    return 1;
  }

  for (int i = 0; i < sh_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }
  return sh_launch(args);
}

void sh_loop(void) {
  char *line;
  char **args;
  int status;

  do {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("%s > ", cwd);

    line = sh_read_line();
    Token *tokens = sh_tokenize(line);
    Command cmd = sh_parse(tokens);
    args = sh_split_line(line);
    status = sh_execute(args);

    free(line);
    free(args);
  } while (status);
}
