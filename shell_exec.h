#ifndef _shell_exec_h
#define _shell_exec_h

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include "3600sh.h"
#include "arg_array.h"
#include "shell_cmds.h"
#include "shell_io.h"

int execute_cmd(status *s);

#endif
