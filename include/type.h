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
    int id;
    int status;
    char username[BUFF_SIZE / 8];
    char password[BUFF_SIZE / 8];
} Client;

typedef struct room {
    int id;
    int users[BUFF_SIZE * 4];
    char name[BUFF_SIZE / 8];
    char moderator[BUFF_SIZE / 8];
} Room;

typedef enum data_t { CLIENT, ROOM } DataType;

typedef union data {
    Client client;
    Room room;
} Data;

int compare_client(const void *a, const void *b);

Data *search_client(const char *username, Data *clients);

#endif  // TYPE_H
