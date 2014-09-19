#include "arg_array.h"

/*
 * Generate a new argument array, initialize it to be empty, and return a pointer to the caller
 */
arg_array *new_arg_array() {
  arg_array *aa = (arg_array *) calloc(1, sizeof(arg_array));
  if (aa == NULL) { // failed allocating space
    printf("Error: Failure during attempt to allocate arg_array space.\n");
    return NULL; // return NULL, unable to allocate space, NULL ptr
  }

  aa->len = 0; // no elements, length is 0
  aa->emnts = NULL; // no elements, set to NULL
  return aa;
}

/*
 * Insert a new argument e at the end of the provided arg_array aa
 */
void insert_arg(arg_array *aa, char *e) {
  aa->emnts = realloc(aa->emnts, (aa->len+1) * sizeof(char *));
  if (aa->emnts == NULL) {
    printf("Error: Failure during attempt to re-allocate space for insert.\n");
  }
  aa->emnts[aa->len++] = e;
}

/*
 * Retrieve the arg_array's array representation
 */
char **retrieve_arg_array(arg_array *aa) {
  return aa->emnts;
}

/*
 * Return the provided arg_array's length
 */
int arg_array_length(arg_array *aa) {
  return aa->len;
}

/*
 * Reset the provided arg_array by freeing the memory used by previous elements and resetting the length
 */
void reset_arg_array(arg_array *aa) {
  aa->len = 0; // new length is 0 (emptying list)
  int i = 0;
  while (aa->emnts[i] != NULL) { // for each non-NULL element
    free(aa->emnts[i++]); // free the memory allocated
  }
  free(aa->emnts); // free space for emnts
  aa->emnts = NULL; // set to null since no list elements
}

/*
 * Delete the provided arg_array by freeing all memory allocated for the elements and struct
 */
void delete_arg_array(arg_array *aa) {
  int i = 0;
  while (aa->emnts[i] != NULL) { // for each non-NULL element
    free(aa->emnts[i++]); // free the memory allocated
  }
  free(aa->emnts); // free space for elements
  free(aa); // free this list
}
