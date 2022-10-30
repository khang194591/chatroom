#ifndef TYPE_H
#define TYPE_H

#include <stdlib.h>
#include <string.h>

#define BUFF_SIZE 256

#define SUCCESS 1
#define FAILED 0

// Auth command
#define CMD_AUTH 0
#define AUTH_LOGIN 0
#define AUTH_REGISTER 1
#define AUTH_LOGOUT 2

// Dev command
#define DEV 99
#define LIST 0

typedef struct client {
    int fd;
    int status;
    char username[BUFF_SIZE / 8];
    char password[BUFF_SIZE / 8];
} Client;

typedef struct response {
    int state;
    char message[BUFF_SIZE];
} Response;

typedef enum data_t { CLIENT, RESPONSE } DataType;

typedef union data {
    Client client;
    Response response;
} Data;

typedef struct array {
    int n;
    Data array[BUFF_SIZE * 4];
} Array;

int compare_client(const void *a, const void *b);

Data *search_client(const char *username, Array clients);

#endif  // TYPE_H
