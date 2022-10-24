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

#define BUFF_SIZE 1024

#define PRINT_ERROR printf("[Error at line %d in %s]: %s\n", __LINE__, __FILE__, strerror(errno))

int prompt_input(char const *message, char *buff);

void append(char **p_src, char *dest);

void clear_line();

int config_server(int *fd, struct sockaddr_in *addr);

#endif  // UTILS_H