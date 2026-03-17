#include "../include/builtins.h"

pid_t shell_pgid;
Job job_table[MAX_JOBS];
int job_count = 0;

//  list of builtin commands, followed by their corresponding functions
char *builtin_str[] = {"cd", "help", "exit", "jobs", "fg", "bg"};
int (*builtin_func[])(char **) = {&sh_cd,   &sh_help, &sh_exit,
                                  &sh_jobs, &sh_fg,   &sh_bg};

int sh_num_builtins() { return sizeof(builtin_str) / sizeof(char *); }

//  builtin function implementations

int sh_cd(char **args) {
  if (args[1] == NULL) {
    fprintf(stderr, "sh: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("sh");
    }
  }
  return 1;
}

int sh_help(char **args) {
  (void)args;

  int i;
  printf("UNIX-Shell\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < sh_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}

int sh_exit(char **args) {
  (void)args;
  return 0;
}

int sh_jobs(char **args) {
  (void)args;
  for (int i = 0; i < job_count; i++) {
    char *status_str = job_table[i].status == RUNNING ? "Running" : "Stopped";
    printf("[%d] %s %d %s\n", i + 1, status_str, job_table[i].pid,
           job_table[i].command);
  }
  return 1;
}

int sh_fg(char **args) {
  int status;
  if (args[1] == NULL) {
    fprintf(stderr, "sh: fg: expected job number\n");
    return 1;
  }

  int job_index = atoi(args[1]) - 1;
  if (job_index < 0 || job_index >= job_count) {
    fprintf(stderr, "sh: fg: no such job");
    return 1;
  }

  pid_t job_pid = job_table[job_index].pid;

  // negative pid sends signal to entire process group
  kill(-job_pid, SIGCONT);

  // give terminal to job so it can receive keyboard input
  tcsetpgrp(STDIN_FILENO, job_table[job_index].pid);

  do {
    waitpid(job_pid, &status, WUNTRACED);
  } while (!WIFEXITED(status) && !WIFSIGNALED(status) && !WIFSTOPPED(status));

  if (WIFEXITED(status) || WIFSIGNALED(status)) {
    // job finished, remove from table;
    job_table[job_index] = job_table[job_count - 1];
    job_count--;
  }

  if (WIFSTOPPED(status)) {
    job_table[job_index].status = STOPPED;
    printf("\n[%d] suspended %s\n", job_index + 1,
           job_table[job_index].command);
  }

  tcsetpgrp(STDIN_FILENO, shell_pgid);

  return 1;
}

int sh_bg(char **args) {
  if (args[1] == NULL) {
    fprintf(stderr, "sh: bg: expected job number\n");
    return 1;
  }

  int job_index = atoi(args[1]) - 1;
  if (job_index < 0 || job_index >= job_count) {
    fprintf(stderr, "sh: bg: no such job");
    return 1;
  }

  pid_t job_pid = job_table[job_index].pid;

  kill(-job_pid, SIGCONT);

  job_table[job_index].status = RUNNING;

  printf("[%d] %d continued %s\n", job_index + 1, job_pid,
         job_table[job_index].command);

  return 1;
}
