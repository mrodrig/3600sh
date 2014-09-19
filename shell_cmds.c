#include "shell_cmds.h"

/*
 * Print the necessary message, wait for the children to exit, then exit the parent process
 */
void do_exit() {
  printf("So long and thanks for all the fish!\n");

  // Wait for all children to exit, then exit
  while (wait(NULL) > 0) {} 
  exit(0);
}
