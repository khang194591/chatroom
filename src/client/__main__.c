#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "io.h"
#include "utils.h"

int fd;

char buff[BUFF_SIZE + 1];

char username[BUFF_SIZE / 8];
char password[BUFF_SIZE / 8];

char res_stt[BUFF_SIZE];
char res_msg[BUFF_SIZE];

void handle_dev() {
    uint choice = 0;
    int EXIT = 3;
    while (choice != EXIT) {
        int flag = 1;
        printf("1. List all account\n2. Register\n3. Exit\n");
        prompt_input("Enter: ", buff);
        choice = strtol(buff, NULL, 10);
        switch (choice) {
            case 1: {
                sprintf(buff, "%d %d", DEV, LIST);
                send(fd, buff, strlen(buff), 0);
                break;
            }
            default: {
                flag = 0;
                break;
            }
        }
        if (flag) {
            memset(buff, 0, strlen(buff));
            if (recv(fd, buff, BUFF_SIZE, 0) != -1) {
                printf("%s\n", buff);
            }
        }
    }
}

void handle_home() {
    uint choice = 0;
    while (choice != 3) {
        int flag = 1;
        system("clear");
        printf("====================== CHAT APP ======================\n");

        printf("1. List all account\n2. Register\n3. Logout\n");
        prompt_input("Enter: ", buff);
        choice = strtol(buff, NULL, 10);
        switch (choice) {
            case 1: {
                sprintf(buff, "%d %d", DEV, LIST);
                send(fd, buff, strlen(buff), 0);
                break;
            }
            case 3: {
                sprintf(buff, "%d %d", CMD_AUTH, AUTH_LOGOUT);
                send(fd, buff, strlen(buff), 0);
                break;
            }
            default: {
                flag = 0;
                break;
            }
        }
        if (flag) {
            memset(buff, 0, strlen(buff));
            if (recv(fd, buff, BUFF_SIZE, 0) != -1) {
                printf("%s\n", buff);
            }
        }
    }
}

void handle_auth() {
    uint choice = 0;

    while (choice != 3) {
        system("clear");
        int flag = 1;
        printf("0. Developer mode\n");
        printf("1. Login\n2. Register\n3. Exit\n");
        prompt_input("Enter: ", buff);
        if (strcmp(buff, "0") == 0) {
            handle_dev();
            continue;
        }
        choice = strtol(buff, NULL, 10);
        switch (choice) {
            case 1:
                prompt_input("Username: ", username);
                prompt_input("Password: ", password);
                sprintf(buff, "%d %d %s %s", 0, 0, username, password);
                send(fd, buff, strlen(buff), 0);
                break;
            case 2:
                prompt_input("Username: ", username);
                prompt_input("Password: ", password);
                sprintf(buff, "%d %d %s %s", 0, 1, username, password);
                send(fd, buff, strlen(buff), 0);
                break;
            default:
                flag = 0;
                break;
        }
        if (flag) {
            bzero(buff, strlen(buff));
            if (recv(fd, buff, BUFF_SIZE, 0) != -1) {
                sscanf(buff, "%s%s", res_stt, res_msg);
                if (strtol(res_stt, NULL, 10)) {
                    handle_home();
                }
                printf("%s\n", buff);
            }
        }
        prompt_input("Enter anything to continue...", buff);
    }
    printf("Bye bye\n");
}

int main(int argc, char *argv[]) {
    struct sockaddr_in server_addr;

    // Validate port
    if (argc != 3) {
        printf("Usage: %s [ADDRESS] [PORT]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    size_t port = strtol(argv[2], NULL, 10);
    if (errno != 0) {
        PRINT_ERROR;
        exit(EXIT_FAILURE);
    }

    // Step 1: Construct socket
    fd = socket(AF_INET, SOCK_STREAM, 0);

    // Step 2: Specify server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);

    // Step 3: Request to connect server
    if (connect(fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0) {
        PRINT_ERROR;
        exit(EXIT_FAILURE);
    }

    // Step 4: Handle connection
    handle_auth();

    close(fd);
    return 0;
}