/**
 * CS3600, Spring 2013
 * Project 1 Starter Code
 * (c) 2013 Alan Mislove
 *
 * You should use this (very simple) starter code as a basis for 
 * building your shell.  Please see the project handout for more
 * details.
 */

#include "3600sh.h"
#include "shell_cmds.h"
#include "system_cmds.h"
#include "shell_io.h"
#include "arg_string.h"
#include "arg_array.h"
#include "shell_exec.h"

#define USE(x) (x) = (x)

int main(int argc, char*argv[]) {
  // Code which sets stdout to be unbuffered
  // This is necessary for testing; do not change these lines
  USE(argc);
  USE(argv);
  setvbuf(stdout, NULL, _IONBF, 0); 

  char *user = getenv("USER"); // Get the current user from the env variables
  char *host = retrieve_hostname(); // Get the current hostname from the system
  char *pwd  = getenv("PWD"); // Get the current directory from the env variables
  status *read_parse_status; // Result from our parsing function(s)
  // Main loop that reads a command and executes it
  while (1) {         
    print_prompt(user, host, pwd); // print the prompt
    read_parse_status = read_and_parse(); // read input, parse it, and return the status struct back. it contains all necessary info we need
    if (read_parse_status->status_code == 0) { // if no errors reading and parsing,
      execute_cmd(read_parse_status); // then execute the cmd
    }

    if (read_parse_status->eof_hit == 1) { // if there are no more cmds,
      break; // exit the loop
    }
    release_status(read_parse_status); // we're done with this set of args, reset it to empty
  }

  do_exit(); // this is only hit if we have hit an EOF mark (no more input)
}
