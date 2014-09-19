#include "arg_string.h"

/*
 * Generate a new argument string, initialize it to be empty, and return a pointer to the caller
 */
arg_string *new_arg_string() {
  arg_string *as = (arg_string *) calloc(1, sizeof(arg_string)); // allocate the struct's memory
  if (as == NULL) {
    printf("Error: Failure during attempt to allocate arg_string space.\n");
    return NULL;
  }

  as->len = 0; // set the length to 0
  as->str = (char *) calloc(as->len + 1, sizeof(char)); // allocate the string value's memory
  if (as->str == NULL) {
    printf("Error: Failure during attempt to allocate arg_string's string space.\n");
    free(as); // if an error occurred, free the memory from the struct to prevent a mem leak
    return NULL;
  }
  as->str[0] = '\0'; // null-byte terminate it

  return as;
}

/*
 * Append a new character c at the end of the provided arg_string as
 */
void arg_string_append(arg_string *as, char c) {
  as->str = realloc(as->str, (as->len+2) * sizeof(char)); // allocate len+2 to accomodate new character and null-byte
  if (as->str == NULL) {
    printf("Error: Failure during attempt to re-allocate space for append.\n");
  }

  as->str[as->len++] = c; // insert new character and then increase the length
  as->str[as->len] = '\0'; // insert the null-byte terminator
}

/*
 * Retrieve the arg_string's string representation
 */
char *retrieve_arg_string(arg_string *as) {
  return as->str;
}

/*
 * Return the provided arg_string's length
 */
int arg_string_length(arg_string *as) {
  return as->len;
}

/*
 * Reset the provided arg_string by freeing the memory allocated for the string value and reset the length
 */
void reset_arg_string(arg_string *as) {
  as->len = 0; // reset the length to 0
  free(as->str); // free the current string value
  as->str = calloc((as->len + 1), sizeof(char)); // allocate a new empty string
  if (as->str == NULL) {
    printf("Error: Failure during attempt to re-allocate space for reset.\n");
  }
  as->str[as->len] = '\0'; // null-byte terminate it
}

/*
 * Delete the provided arg_string by freeing all memory allocated for the string val and struct
 */
void delete_arg_string(arg_string *as) {
  free(as->str);
  free(as);
}
