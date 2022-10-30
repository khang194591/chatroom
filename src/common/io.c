#include "io.h"

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

void write_to_file(const char *filename, Array array, DataType type) {
    FILE *file = fopen(filename, "w");
    int n = array.n;
    Data *data = array.array;
    for (int i = 0; i < n; ++i) {
        if (type == CLIENT) {
            fprintf(file, "%s %s %d\n", data[i].client.username, data[i].client.password, data[i].client.status);
        }
    }
    fclose(file);
}

void read_from_file(const char *filename, Array *clients, DataType type) {
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
    clients->n = count;
    for (int i = 0; i < count; ++i) {
        fgets(buff, BUFF_SIZE, file);
        if (type == CLIENT) {
            sscanf(buff, "%s%s%d", clients->array[i].client.username, clients->array[i].client.password,
                   &clients->array[i].client.status);
        }
    }
//    for (int i = 0; i < clients->n; ++i) {
//        Client temp = clients->array[i].client;
//        printf("%s %s\n", temp.username, temp.password);
//    }
    fclose(file);
}
