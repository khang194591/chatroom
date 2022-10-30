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

JRB array_client;

char buff[BUFF_SIZE + 1];
char username[BUFF_SIZE];
char password[BUFF_SIZE];
char buff_cmd[BUFF_SIZE];
char buff_cmd_t[BUFF_SIZE];
char response[BUFF_SIZE];

char *body = NULL;

// DONE
void handle_exit(int sig) {
    clear_line();
    printf("Saving data for next use\n");
    write_to_file("data/client.txt", array_client, CLIENT);
    exit(EXIT_SUCCESS);
}

// DONE
void handle_auth(Client *client) {
    sscanf(body, "%s%s", username, password);
    size_t cmd_t = strtol(buff_cmd_t, NULL, 10);
    switch (cmd_t) {
        case AUTH_LOGIN: {
            JRB node = jrb_find_str(array_client, username);
            if (node == NULL) {
                sprintf(response, "%d %s", FAILED, "Invalid username or password");
            } else {
                Client *temp = (Client *)jval_s(node->val);
                if (strcmp(temp->password, password) != 0) {
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
            JRB node = jrb_find_str(array_client, username);
            if (node == NULL) {
                Client *temp = malloc(sizeof(Client));
                strcpy(temp->username, username);
                strcpy(temp->password, password);
                client->status = 0;
                jrb_insert_str(array_client, temp->username, new_jval_s((char *)temp));
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

// DEVELOPMENT:
void handle_dev() {
    size_t cmd_t = strtol(buff_cmd_t, NULL, 10);
    switch (cmd_t) {
        // List all user's username and password: [username] [password]
        case LIST: {
            char *buffer = NULL;
            JRB node;
            jrb_traverse(node, array_client) {
                Client client = *(Client *)jval_s(node->val);
                sprintf(buff, "%s %s\n", client.username, client.password);
                append(&buffer, buff);
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
}

void *handle_connection(void *arg) {
    Client client = *(Client *)arg;
    size_t bytes_sent, bytes_received;

    pthread_detach(pthread_self());

    while (1) {
        // Clear all buffer
        bzero(buff, strlen(buff));
        bzero(username, strlen(username));
        bzero(password, strlen(password));
        bzero(buff_cmd, strlen(buff_cmd));
        bzero(buff_cmd_t, strlen(buff_cmd_t));
        bzero(response, strlen(response));

        // Receive incoming message
        bytes_received = recv(client.fd, buff, BUFF_SIZE, 0);
        if (bytes_received == -1) {
            PRINT_ERROR;
        } else if (bytes_received == 0) {
            break;
        }

        // DEVELOPMENT: Handle remote shutdown command
        if (strcmp(buff, "exit") == 0) {
            handle_exit(0);
        }

        // Request: [CMD] [CMD_T] [BODY]
        sscanf(buff, "%s%s", buff_cmd, buff_cmd_t);
        body = buff + strlen(buff_cmd) + strlen(buff_cmd_t) + 2;
        switch (strtol(buff_cmd, NULL, 10)) {
            case CMD_AUTH: {
                handle_auth(&client);
                break;
            }
            // DEVELOPMENT:
            case DEV: {
                handle_dev();
                break;
            }
            default: {
                sprintf(response, "%d %s", FAILED, "Invalid request!");
                break;
            }
        }
        // Send response to client
        bytes_sent = send(client.fd, response, strlen(response), 0);
        if (bytes_sent == -1) {
            PRINT_ERROR;
        }
    }
    free(arg);
    // Close connection
    close(client.fd);
    return NULL;
}

int main(int argc, char *argv[]) {
    // Handle exit by ctrl+C
    signal(SIGINT, handle_exit);

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
    array_client = make_jrb();
    read_from_file("data/client.txt", array_client, CLIENT);

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