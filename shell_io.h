#ifndef _shell_io_h
#define _shell_io_h

#include "3600sh.h"
#include "arg_string.h"
#include "arg_array.h"

typedef struct status_s {
  int parse_mode;
  int escape_mode;
  int bg_proc;
  int redirect_type;
  int io_redirected;
  int status_code;
  int eof_hit;
  arg_array *argv;
  arg_string *arg_string;
  char *fds[3];
} status;


void print_prompt(char *user, char *host, char *pwd);
status *generate_status();
status *read_and_parse();
void parse_input(status *s, char c);
void release_status(status *s);

#endif 
