#ifndef _arg_array_h
#define _arg_array_h

#include "3600sh.h"

struct arg_array_s;

typedef struct arg_array_s {
  int len;
  char **emnts;
} arg_array;

arg_array *new_arg_array();
void insert_arg(arg_array *aa, char *arg);
char **retrieve_arg_array(arg_array *aa);
int arg_array_length(arg_array *aa);
void reset_arg_array(arg_array *aa);
void delete_arg_array(arg_array *aa);

#endif 
