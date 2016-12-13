#pragma once
#include "defines.h"
#include <sys/types.h>
#include <sys/dir.h>
#include <dirent.h>
#include <sys/param.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/xattr.h>
#include "filer.c"

char* execute_cmd(char *input);
void define_root(void);
