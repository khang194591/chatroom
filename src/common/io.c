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
            Client *client = (Client *)jval_s(node->val);
            fprintf(file, "%s %s %d\n", client->username, client->password, client->status);
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
            jrb_insert_str(arr, client->username, new_jval_s((char *)client));
        }
    }
    fclose(file);
}
