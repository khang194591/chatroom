#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INVALID_SOCKET -1

int main(int argc, char const *argv[]) {
    char *domain = argv[1];
    struct hostent *phost = gethostbyname(domain);

    if (phost != NULL) {
        struct hostent host = *phost;
        if (host.h_addrtype == AF_INET) {
            int count = 0;
            struct in_addr addr;
            while (host.h_addr_list[count] != NULL) {
                char *ip_byptes = host.h_addr_list[count++];
                memcpy(&addr, ip_byptes, sizeof(addr));
                char *saddr = inet_ntoa(addr);
                printf("%s\n", saddr);
            }
            if (count > 0) {
                int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
                if (fd != INVALID_SOCKET) {
                    struct sockaddr_in addrin;
                    addrin.sin_family = AF_INET;
                    addrin.sin_port = htons(5000);
                    addrin.sin_addr = addr;
                    if (connect(fd, (struct sockaddr *)&addrin, sizeof(addrin)) == 0) {
                        char buff[1024] = {0};
                        if (send(fd, "Hello", 255 - 1, 0) == 0) {
                        }
                    }
                }
            }
        }
    }

    return 0;
}
