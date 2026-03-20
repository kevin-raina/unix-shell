#include "../include/parser.h"

#include <fcntl.h>
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

void sh_apply_redirections(Command command) {
  if (command.redirect_in != NULL) {
    int fd = open(command.redirect_in, O_RDONLY);
    if (fd == -1) {
      perror("open");
      exit(EXIT_FAILURE);
    }
    dup2(fd, STDIN_FILENO);
    close(fd);
  }

  if (command.redirect_out != NULL) {
    int fd = open(command.redirect_out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
      perror("open");
      exit(EXIT_FAILURE);
    }
    dup2(fd, STDOUT_FILENO);
    close(fd);
  }

  if (command.append != NULL) {
    int fd = open(command.append, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd == -1) {
      perror("open");
      exit(EXIT_FAILURE);
    }
    dup2(fd, STDOUT_FILENO);
    close(fd);
  }
}

int sh_launch(Command command) {
  // ignore SIGTTOU signal
  signal(SIGTTOU, SIG_IGN);

  pid_t pid;
  int status;

  pid = fork();
  if (pid == 0) {
    // child process
    // child becomes its own process group
    setpgid(0, 0);
    // restore default behavior
    signal(SIGINT, SIG_DFL);
    signal(SIGTSTP, SIG_DFL);

    sh_apply_redirections(command);

    if (execvp(command.args[0], command.args) == -1) {
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
    if (command.background == 0) {
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
        store_command(command.args);
        job_table[job_count].status = STOPPED;
        job_count++;
        printf("[%d] %d suspended %s\n", job_count, pid, command.args[0]);
      }

      // take terminal back
      tcsetpgrp(STDIN_FILENO, shell_pgid);
    }

    else {
      // background: Do not wait, print pid and return
      job_table[job_count].pid = pid;
      store_command(command.args);
      job_table[job_count].status = RUNNING;
      job_count++;
      printf("[%d] %d\n", job_count, pid);
    }
  }
  return 1;
}

int sh_execute(Command command) {
  if (command.args[0] == NULL) {
    // an empty command was entered
    return 1;
  }

  for (int i = 0; i < sh_num_builtins(); i++) {
    if (strcmp(command.args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(command.args);
    }
  }
  return sh_launch(command);
}

void sh_loop(void) {
  char *line;
  int status;

  do {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("%s > ", cwd);

    line = sh_read_line();
    Token *tokens = sh_tokenize(line);
    Command cmd = sh_parse(tokens);
    status = sh_execute(cmd);

    free(line);
  } while (status);
}
