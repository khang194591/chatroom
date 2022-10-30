#ifndef IO_H
#define IO_H

#include "stdio.h"
#include "string.h"
#include "type.h"
#include "jrb.h"

void prompt_input(char const *message, char *buff);

void write_to_file(const char *filename, JRB arr, DataType type);

void read_from_file(const char *filename, JRB arr, DataType type);

#endif  // IO_H
