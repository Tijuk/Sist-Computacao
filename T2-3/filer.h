#pragma once
#define _GNU_SOURCE
#include <stdio.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h> 
#include <unistd.h> 
#include <sys/mman.h> 
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/param.h>
#include <errno.h>
#include <sys/xattr.h>


int file_manipulation(int ID, int code, char *path, char *payload, int *nrbytes, int offset);
char* scpy(char *s2);
