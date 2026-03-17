#include "../include/shell.h"

int main(int argc, char **argv) {
  (void)argc;
  (void)argv;

  // ignore SIGTTOU signal
  signal(SIGTTOU, SIG_IGN);

  // save process id
  pid_t sh_pid = getpid();

  // put the shell in it's own process group
  setpgid(sh_pid, sh_pid);

  // save process group id
  shell_pgid = sh_pid;

  // take ownership of terminal if we are in foreground
  if (tcgetpgrp(STDIN_FILENO) == getppid()) {
    tcsetpgrp(STDIN_FILENO, shell_pgid);
  }

  // ignore signals before fork
  signal(SIGINT, SIG_IGN);
  signal(SIGTSTP, SIG_IGN);

  // call handler when child finished
  signal(SIGCHLD, sigchld_handler);

  // load config files, if any

  // run command loop.
  sh_loop();

  // perform any shutdown/cleanup

  return EXIT_SUCCESS;
}
