#ifndef UTILS_H
#define UTILS_H

#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <termios.h>
#include <time.h>

#define PRINT_ERROR printf("[Error at line %d in %s]: %s\n", __LINE__, __FILE__, strerror(errno))

void append(char **p_src, char *dest);

void clear_line();

#endif  // UTILS_H