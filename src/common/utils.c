#include "utils.h"

#include <signal.h>

#include "io.h"

void append(char **p_src, char *dest) {
    char *src = *p_src;
    size_t old_len = src == NULL ? 0 : strlen(src);
    *p_src = (char *)realloc(*p_src, old_len + strlen(dest) + 1);
    src = *p_src;
    memset(src + old_len, 0, strlen(dest) + 1);
    sprintf(src + old_len, "%s", dest);
}

void clear_line() { printf("\33[2K\r"); }
