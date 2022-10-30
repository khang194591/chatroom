#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "io.h"
#include "utils.h"

#define MAX_CLIENT 128

Array array_client;

char buff[BUFF_SIZE + 1];
char username[BUFF_SIZE];
char password[BUFF_SIZE];
char c_buff[BUFF_SIZE];
char t_buff[BUFF_SIZE];
char response[BUFF_SIZE];

char *body = NULL;

void exit_handler(int sig) {
    clear_line();
    printf("Saving data for next use\n");
    write_to_file("data/client.txt", array_client, CLIENT);
    exit(EXIT_SUCCESS);
}

void handle_auth(Client *client) {
    sscanf(body, "%s%s", username, password);
    size_t command_t = strtol(t_buff, NULL, 10);
    switch (command_t) {
        case AUTH_LOGIN: {
            Data *u = search_client(username, array_client);
            if (u == NULL) {
                sprintf(response, "%d %s", FAILED, "Invalid username or password");
            } else {
                if (strcmp(u->client.password, password) != 0) {
                    sprintf(response, "%d %s", FAILED, "Invalid username or password");
                } else {
                    strcpy(client->username, username);
                    strcpy(client->password, password);
                    sprintf(response, "%d %s", SUCCESS, "ok");
                }
            }
            break;
        }
        case AUTH_REGISTER: {
            Data *u = search_client(username, array_client);
            if (u == NULL) {
                strcpy(client->username, username);
                strcpy(client->password, password);
                array_client.array[array_client.n++].client = *client;
                sprintf(response, "%d %s", SUCCESS, "ok");
            } else {
                sprintf(response, "%d %s", FAILED, "Username already taken. Try another one!");
            }
            break;
        }
        case AUTH_LOGOUT: {
            bzero(client->username, BUFF_SIZE / 8);
            bzero(client->password, BUFF_SIZE / 8);
            client->status = 0;
            sprintf(response, "%d %s", SUCCESS, "ok");
            break;
        }
        default: {
            sprintf(response, "%d %s", FAILED, "Invalid request!");
            break;
        }
    }
}

void *handle_connection(void *arg) {
    Client client = *(Client *)arg;
    size_t bytes_sent, bytes_received;

    pthread_detach(pthread_self());

    while (1) {
        bzero(buff, strlen(buff));
        bzero(username, strlen(username));
        bzero(password, strlen(password));
        bzero(c_buff, strlen(c_buff));
        bzero(t_buff, strlen(t_buff));
        bzero(response, strlen(response));

        bytes_received = recv(client.fd, buff, BUFF_SIZE, 0);
        if (bytes_received == -1) {
            PRINT_ERROR;
        } else if (bytes_received == 0) {
            break;
        }

        if (strcmp(buff, "exit") == 0) {
            exit_handler(0);
        }

        sscanf(buff, "%s%s", c_buff, t_buff);
        body = buff + strlen(c_buff) + strlen(t_buff) + 2;
        switch (strtol(c_buff, NULL, 10)) {
            case CMD_AUTH: {
                handle_auth(&client);
                break;
            }
            case DEV: {
                size_t command_t = strtol(t_buff, NULL, 10);
                switch (command_t) {
                    case LIST: {
                        char line[BUFF_SIZE];
                        char *buffer = NULL;
                        for (int i = 0; i < array_client.n; ++i) {
                            Client temp = array_client.array[i].client;
                            sprintf(line, "%s %s\n", temp.username, temp.password);
                            //                            printf("%s %s\n", temp.username, temp.password);
                            append(&buffer, line);
                        }
                        sprintf(response, "%s", buffer);
                        free(buffer);
                        buffer = NULL;
                        break;
                    }
                    default: {
                        break;
                    }
                }
                break;
            }
            default: {
                sprintf(response, "%d %s", FAILED, "Invalid request!");
                break;
            }
        }

        bytes_sent = send(client.fd, response, strlen(response), 0);
        if (bytes_sent == -1) {
            PRINT_ERROR;
        }
    }
    close(client.fd);
    printf("%s disconnected\n", client.username);
    return NULL;
}

int main(int argc, char *argv[]) {
    // Handle exit by ctrl+C
    signal(SIGINT, exit_handler);

    int server_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t sin_size;
    pthread_t tid;

    // Validate port
    if (argc != 2) {
        printf("Usage: %s [PORT]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    size_t port = strtol(argv[1], NULL, 10);
    if (errno != 0) {
        PRINT_ERROR;
        exit(EXIT_FAILURE);
    }
    read_from_file("data/client.txt", &array_client, CLIENT);

    // Step 1: Construct a TCP socket to listen connection request
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        PRINT_ERROR;
        exit(EXIT_FAILURE);
    }

    // Step 2: Bind address to socket
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        PRINT_ERROR;
        exit(EXIT_FAILURE);
    }

    // Step 3: Listen request from client_addr
    if (listen(server_fd, MAX_CLIENT) == -1) {
        PRINT_ERROR;
        exit(EXIT_FAILURE);
    }
    printf("Server is running at port %d\n", ntohs(server_addr.sin_port));

    //    Step 4 : Communicate with client_addr
    while (1) {
        Client *client = malloc(sizeof(Client));
        client->fd = accept(server_fd, (struct sockaddr *)&client_addr, &sin_size);
        if (client->fd == -1) {
            PRINT_ERROR;
            continue;
        }
        printf("You got a connection from %s\n", inet_ntoa(client_addr.sin_addr));
        pthread_create(&tid, NULL, &handle_connection, client);
    }
}