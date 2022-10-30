#include "type.h"

#include <search.h>

int compare_client(const void *a, const void *b) {
    Data A = *(Data *)a;
    Data B = *(Data *)b;
    return strcmp(A.client.username, B.client.username);
}
Data *search_client(const char *username, Data *clients) {
    Data client;
    strcpy(client.client.username, username);
    return lfind(&client, clients, (size_t *)&clients, sizeof(Data), compare_client);
}