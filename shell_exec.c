#include <fcntl.h>
#include <stdio.h>
#include "shell_exec.h"
#include "3600sh.h"
#include "shell_io.h"

#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

/*
 * Execute the command that the user entered as provided by the status.
 * The status indicates information such as whether this is a background proc, fds, cmd, etc.
 */
int execute_cmd(status *s) {
  char **exec_argv = retrieve_arg_array(s->argv); // retrieve the array representation of the arg_array
  if (*exec_argv == NULL) { // if no commands, do nothing
    return 0;
  } else if (strcmp(*exec_argv, "exit") == 0) { // if the user entered 'exit' then exit
    do_exit();
    return 0;
  }

  // Store copies of the original file descriptors so we can switch back to them afterwards
  int stdin_fd_alias  = dup(STDIN_FILENO);
  int stdout_fd_alias = dup(STDOUT_FILENO);
  int stderr_fd_alias = dup(STDERR_FILENO);

  int temp_fd; // temporary file descriptor value for the swapping of file descriptors
  if (s->fds[STDIN_FILENO]) { // if we have an input redirection
    temp_fd = open(s->fds[STDIN_FILENO], O_RDONLY); // open the file for reading as input
    if (temp_fd == -1) {
      printf("Error: Unable to open redirection file.\n"); // print required msg
      return 1;
    }
    dup2(temp_fd, STDIN_FILENO); // swap the opened stdin redirect file with stdin
  }
  if (s->fds[STDOUT_FILENO]) { // if we have an output redirection
    temp_fd = open(s->fds[STDOUT_FILENO], O_WRONLY | O_TRUNC | O_CREAT, 0777); // open file, create/overwrite contents, permissions: 777
    if (temp_fd == -1) {
      printf("Error: Unable to open redirection file.\n"); // print required msg
      return 1;
    }
    dup2(temp_fd, STDOUT_FILENO); // swap the opened stdout redirect file with stdout
  }
  if (s->fds[STDERR_FILENO]) { // if we have a stderr output redirection
    temp_fd = open(s->fds[STDERR_FILENO], O_WRONLY | O_TRUNC | O_CREAT, 0777); // open file, create/overwrite contents, permissions: 777
    if (temp_fd == -1) {
      printf("Error: Unable to open redirection file.\n"); // print required msg
      return 1;
    }
    dup2(temp_fd, STDERR_FILENO); // swap the opened stderr redirect file with stderr
  }

  pid_t pid = fork(); // fork and capture the PID

  if (pid < 0) {  // if pid < 0, then fork failed.
    printf("Error: Failed trying to fork.\n");
    return 1;
  }

  if (pid == 0) { // child process code
    if (execvp(exec_argv[0], exec_argv) == -1) { // if there was an error with execvp
      if (errno == EACCES) { // if an error is thrown trying execvp
        printf("Error: Permission denied.\n");
      } else {
        printf("Error: Command not found.\n");
      }
    }
    exit(1); // have child process exit
  } else { // parent process code
    // swap parent's file descriptors back to normal before returning
    // note: this was the only way I could figure out how to do it.. trying to swap the fds for just the child caused things to print multiple times.
    dup2(stdin_fd_alias,  STDIN_FILENO);  
    dup2(stdout_fd_alias, STDOUT_FILENO);
    dup2(stderr_fd_alias, STDERR_FILENO);

    if (!s->bg_proc) { // if this is not a background process then wait for it to exit before returning
      waitpid(pid, NULL, 0);
    }
    return 0;
  }
}
