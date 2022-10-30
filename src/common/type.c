#include "type.h"

#include <search.h>

int compare_client(const void *a, const void *b) {
    Data A = *(Data *)a;
    Data B = *(Data *)b;
    return strcmp(A.client.username, B.client.username);
}
Data *search_client(const char *username, Array clients) {
    Data client;
    strcpy(client.client.username, username);
    return lfind(&client, clients.array, (size_t *)&clients.n, sizeof(Data), compare_client);
}