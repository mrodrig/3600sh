#ifndef _arg_string_h
#define _arg_string_h

#include "3600sh.h"

struct arg_string_s;

typedef struct arg_string_s {
  int len;
  char *str;
} arg_string;

arg_string *new_arg_string();
void arg_string_append(arg_string *as, char c);
char *retrieve_arg_string(arg_string *as);
int arg_string_length(arg_string *as);
void reset_arg_string(arg_string *as);
void delete_arg_string(arg_string *as);

#endif 
