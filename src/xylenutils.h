#ifndef XYLENUTILS_H
#define XYLENUTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>

#define XU_SUCCESS 0
#define XU_ERROR 1

// Common utility functions
void xu_error(const char *fmt, ...);
int xu_copy_file(const char *src, const char *dst);
int xu_is_directory(const char *path);
int xu_check_help(int argc, char **argv, const char *usage);

#endif
