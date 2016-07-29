#ifndef UTIL_FILESYSTEM_H
#define UTIL_FILESYSTEM_H

#include <iostream>
#include <sstream>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <libgen.h>

void mkParents(char *path, mode_t mode);

#endif
