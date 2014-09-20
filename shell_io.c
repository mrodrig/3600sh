#include "shell_io.h"

#define TRUE       1
#define FALSE      0

#define NO_REDIR  -1
#define INP_REDIR  0
#define OUT_REDIR  1
#define ERR_REDIR  2

/*
 * Print the shell prompt
 */
void print_prompt(char *user, char *host, char *pwd) {
  printf("%s@%s:%s> ", user, host, pwd);
  fflush(stdout); // force writing to stdout
}

/*
 * Initialize a new status with the defaults that we want for input reading/parsing/execution
 */
status *generate_status() {
  status *s = (status *) calloc(1, sizeof(status));
  if (s == NULL) {
    s->status_code = 3; // failed to allocate the memory for our status struct
    return NULL;
  }
  s->arg_string = new_arg_string();
  s->argv = new_arg_array();
  for(int i=0; i<3; i++) { // Initialize file descriptors values to NULL to avoid bad data being accessed
    s->fds[i] = NULL;
  }
  s->parse_mode     = TRUE;      // default: parse_mode        = 1 -- are we currently parsing input from the user?
  s->bg_proc        = FALSE;     // default: s->bg_proc        = 0 -- is this process going to run in the background?
  s->redirect_type  = NO_REDIR;  // default: s->redirect_type  =-1 -- indicates which type of redirection we are dealing with; if -1, not currently dealing with redirect
  s->io_redirected  = FALSE;     // default: s->io_redirected  = 0 -- how many redirect symbols have we seen so far?
  s->escape_mode    = FALSE;     // default: s->escape_mode    = 0 -- are we currently dealing with an escaped character?
  s->eof_hit        = FALSE;     // default: s->eof_hit        = 0 -- have we hit the eof marker?
  s->status_code    = 0;         // default: s->status_code    = 0 -- what code should we return to the caller?
  return s;
}

/*
 * Read input from the user, parse the result and return it
 */
status *read_and_parse() {
  char input;
  status *s = generate_status();

  while (input != '\n' && input != EOF) { // while we are still receiving input
    input = getchar(); // retrieve a character from the user
    parse_input(s, input);
  }

  if (input == EOF) {
    s->eof_hit = 1;
  }

  if (s->status_code == 1) {
    printf("Error: Unrecognized escape sequence.\n");
  } else if (s->status_code == 2) {
    printf("Error: Invalid syntax.\n");
  } else if (s->status_code == 3) {
    printf("Error: Failure during attempt to allocate status space.\n");
  }
  insert_arg(s->argv, NULL); // insert NULL at end so we know what end is
  return s;
}

/*
 * Parse the input to determine exactly what needs to be setup for the process to execute as intended
 */
void parse_input(status *s, char c) {
  if (!s->escape_mode) { // if we aren't in escape mode
    if (c == ' ' || c == '\t' || c == '\n' || c == EOF) { // end of line or arg delimiters
      if (s->redirect_type == NO_REDIR) { // if we haven't seen any redirects
        if ((strcmp("<", retrieve_arg_string(s->arg_string))) == 0) { // if current arg is the inp redir char
          s->io_redirected = TRUE; // store that we are redirecting io
          s->redirect_type = INP_REDIR; // set redirect type to c
        } else if ((strcmp(">", retrieve_arg_string(s->arg_string))) == 0) {
          s->io_redirected = TRUE; // store that we are redirecting io
          s->redirect_type = OUT_REDIR; // set redirect type to output
        } else if ((strcmp("2>", retrieve_arg_string(s->arg_string))) == 0) {
          s->io_redirected = TRUE; // store that we are redirecting io
          s->redirect_type = ERR_REDIR; // set redirect type to output
        } else if (arg_string_length(s->arg_string)) { // if we have an arg
          char *arg = calloc(arg_string_length(s->arg_string), sizeof(char)); // allocate a permenant address for this arg string
          if (arg == NULL) { // if the calloc fails,
            s->status_code = 3; // set a failed alloc status code
            s->parse_mode = FALSE; // stop parsing
          } else if (s->io_redirected) { // if IO is redirected, then this is the arg immediately after the redir char
            s->status_code = 2; // illegal syntax
            s->parse_mode = FALSE; // stop parsing
          } else { // otherwise,
            sprintf(arg, "%s", retrieve_arg_string(s->arg_string)); // copy the arg into the mem location
            insert_arg(s->argv, arg); // insert the pointer into our arg_array
          }
        }
        // if we have a redirect and the fds array is already populated or there is no arg after the redir char
        if (s->redirect_type != NO_REDIR && ((s->fds[s->redirect_type] != NULL) || (c == '\n' || c == EOF))) {
          s->status_code = 2; // illegal syntax
          s->parse_mode = FALSE; // stop parsing
        }
      } else if (arg_string_length(s->arg_string)) { // if we have an argument
        char* as = retrieve_arg_string(s->arg_string); // retrieve it
        s->fds[s->redirect_type] = calloc(arg_string_length(s->arg_string), sizeof(char)); // allocate space for the filename
        if ((strcmp("<", as) == 0) || // if the arg is an inp redir char
            (strcmp(">", as) == 0) || // if the arg is an outp redir char
            (strcmp("2>", as) == 0)) { // if the arg is a stderr redir char
          s->status_code = 2; // illegal syntax
          s->parse_mode = FALSE; // stop parsing
        } else if (s->fds[s->redirect_type] == NULL) {
          s->status_code = 3; // if the calloc fails, set a failed alloc status code
          s->parse_mode = FALSE; // stop parsing
        } else {
          sprintf(s->fds[s->redirect_type], "%s", retrieve_arg_string(s->arg_string)); // copy the file name into the fds array
        }
        s->redirect_type = NO_REDIR; // reset the redirect type
      } else if (c == '\n' || c == EOF) {
        s->status_code = 2;
        s->parse_mode = FALSE;
      }
      reset_arg_string(s->arg_string);
    } else if (c == '\\') { // if we just saw a backslash, then set escape_mode so we check the next character
      s->escape_mode = 1; 
    } else if (c == '&' && s->bg_proc) { // if we have already seen an '&' to denote this is a bg process and saw another, illegal
      s->status_code = 2; // mark this as illegal syntax
      s->parse_mode = FALSE; // stop parsing
    } else if (c == '&') { // haven't seen a '&' yet,
      s->bg_proc = 1; // set this as a background process
    } else if (s->bg_proc) {
      s->status_code = 2; // illegal syntax
      s->parse_mode = FALSE; // stop parsing
    } else { // otherwise, append the c to our arg_string
      arg_string_append(s->arg_string, c); 
    }
  } else { // in escape mode
    if (c == ' ' || c == '&' || c == '\\') { // if we see these legal characters, we can append them exactly as is
      arg_string_append(s->arg_string, c);
    } else if (c == 't') { // else, if we see a t, then we need to append a tab character
      arg_string_append(s->arg_string, '\t');
    } else {
      s->status_code = 1; // unrecognized escape sequence
      s->parse_mode = FALSE; // stop parsing
    }
    s->escape_mode = FALSE; // we're done in escape mode - saw character after initial '\\'
  }
}

/*
 * Once the 3600sh main function is done with the status that read_and_parse returns, it calls this function.
 * This function frees all memory that has been allocated for the struct and its components
 */
void release_status(status *s) {
  delete_arg_string(s->arg_string); // free mem used by the arg string temp var
  delete_arg_array(s->argv); // free mem used by argv
  free(s); // free this status
}
