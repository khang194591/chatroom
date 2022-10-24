#include "utils.h"

#include <signal.h>

int prompt_input(char const *message, char *buff) {
    memset(buff, 0, BUFF_SIZE);
    printf("%s", message);
    fgets(buff, BUFF_SIZE, stdin);
    int index;
    for (index = 0; index < BUFF_SIZE; ++index) {
        if (buff[index] == '\n' || buff[index] == '\0') {
            buff[index] = '\0';
            return index;
        }
    }
    return -1;
}
void append(char **p_src, char *dest) {
    char *src = *p_src;
    size_t old_len = src == NULL ? 0 : strlen(src);
    *p_src = (char *)realloc(*p_src, old_len + strlen(dest) + 1);
    src = *p_src;
    memset(src + old_len, 0, strlen(dest) + 1);
    sprintf(src + old_len, "%s", dest);
}

void clear_line() { printf("\33[2K\r"); }

int config_server(int *fd, struct sockaddr_in *addr) {
    char buff[BUFF_SIZE];
    *fd = socket(AF_INET, SOCK_STREAM, 0);
    if (!*fd) {
        return -1;
    }
    addr->sin_family = AF_INET;
    prompt_input("Input server address: ", buff);
    if (strcmp(buff, "\n") == 0 || strcmp(buff, "\0") == 0) {
        strcpy(buff, "127.0.0.1");
    }
    addr->sin_addr.s_addr = inet_addr(buff);
    prompt_input("Input server port: ", buff);
    if (strcmp(buff, "\n") == 0 || strcmp(buff, "\0") == 0) {
        strcpy(buff, "3000");
    }
    addr->sin_port = strtol(buff, NULL, 10);
    return 0;
}
