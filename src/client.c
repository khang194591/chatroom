#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "jrb.h"
#include "jval.h"
#include "utils.h"

typedef struct client {
    // struct sockaddr_in addr;
    int fd;
    int id;
    char *username;
} * Client;

Client make_client(/* struct sockaddr_in addr, */ int fd, int id, char *username) {
    Client client = malloc(sizeof(struct client));
    // client->addr = addr;
    client->fd = fd;
    client->id = id;
    client->username = NULL;
    append(&client->username, username);
    return client;
}

int main(int argc, char const *argv[]) { return 0; }
