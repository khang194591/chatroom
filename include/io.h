#ifndef IO_H
#define IO_H

#include "stdio.h"
#include "string.h"
#include "type.h"

void prompt_input(char const *message, char *buff);

void write_to_file(const char *filename, Array array, DataType type);

void read_from_file(const char *filename, Array *clients, DataType type);

#endif  // IO_H
