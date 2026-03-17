#include "../include/jobs.h"

void sigchld_handler(int sig) {
  (void)sig;
  int status;
  pid_t pid;

  while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
    for (int i = 0; i < job_count; i++) {
      if (job_table[i].pid == pid) {
        job_table[i] = job_table[job_count - 1];
        job_count--;
        break;
      }
    }
  }
}

void store_command(char **args) {
  // store full command string
  job_table[job_count].command[0] = '\0';
  for (int i = 0; args[i] != NULL; i++) {
    strncat(job_table[job_count].command, args[i], 255);
    if (args[i + 1] != NULL)
      strncat(job_table[job_count].command, " ", 255);
  }
}
