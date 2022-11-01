#include "io.h"

#include "jrb.h"

void prompt_input(char const *message, char *buff) {
    memset(buff, 0, BUFF_SIZE);
    printf("%s", message);
    fgets(buff, BUFF_SIZE, stdin);
    int index;
    for (index = 0; index < BUFF_SIZE; ++index) {
        if (buff[index] == '\n' || buff[index] == '\0') {
            buff[index] = '\0';
        }
    }
}

void write_to_file(const char *filename, JRB arr, DataType type) {
    FILE *file = fopen(filename, "w");
    JRB node;
    jrb_traverse(node, arr) {
        if (type == CLIENT) {
            Client *client = jval_v(node->val);
            fprintf(file, "%s %s %d\n", client->username, client->password, client->status);
        } else if (type == ROOM) {
            Room *room = jval_v(node->val);
            fprintf(file, "%s %s ", room->name, room->moderator);
            JRB user;
            jrb_traverse(user, room->users) { fprintf(file, "%s=", jval_s(user->val)); }
            fprintf(file, "\n");
        }
    }
    fclose(file);
}

void read_from_file(const char *filename, JRB arr, DataType type) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        return;
    }
    char buff[BUFF_SIZE];
    int count = 0;
    while (fgets(buff, 255, file) != 0) {
        count++;
    }
    rewind(file);
    for (int i = 0; i < count; ++i) {
        fgets(buff, BUFF_SIZE, file);
        if (type == CLIENT) {
            Client *client = malloc(sizeof(Client));
            sscanf(buff, "%s%s%d", client->username, client->password, &client->status);
            jrb_insert_str(arr, client->username, new_jval_v(client));
        } else if (type == ROOM) {
            char users[BUFF_SIZE];
            Room *room = malloc(sizeof(Room));
            sscanf(buff, "%s%s%s", room->name, room->moderator, users);
            room->users = make_jrb();
            char *token;

            /* get the first token */
            token = strtok(users, "=");

            /* walk through other tokens */
            while (token != NULL) {
                jrb_insert_str(room->users, strdup(token), new_jval_s(strdup(token)));
                token = strtok(NULL, "=");
            }
            jrb_insert_str(arr, room->moderator, new_jval_v(room));
            free(token);
            token = NULL;
        }
    }
    fclose(file);
}