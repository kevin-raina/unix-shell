#pragma once

#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_JOBS 64

typedef enum {
  RUNNING,
  STOPPED,
  DONE,
} JobStatus;

typedef struct {
  pid_t pid;
  char command[256];
  JobStatus status;
} Job;

extern Job job_table[MAX_JOBS];
extern int job_count;
extern pid_t shell_pgid;

void sigchld_handler(int sig);
void store_command(char **args);
