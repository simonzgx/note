//
// Created by simon on 2020/3/15.
//

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define LOCAL_HOST "127.0.0.1"
#define PORT 8080

int main() {
    int new_sock;
    struct sockaddr_in servaddr{};

    // socket create and varification
    new_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (new_sock == -1) {
        printf("socket creation failed...\n");
        exit(0);
    } else
        printf("Socket successfully created..\n");

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(LOCAL_HOST);
    servaddr.sin_port = htons(PORT);

    if (connect(new_sock, (struct sockaddr *) &servaddr, sizeof(servaddr)) != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    } else
        printf("connected to the server..\n");

    char buff[1024];
    int n;
    for (;;) {
        memset(buff, 0, sizeof(buff));
        printf("Input the data to send: ");
        n = 0;
        while ((buff[n++] = getchar()) != '\n');

        if ((strncmp(buff, "exit", 4)) == 0) {
            printf("Exit...\n");
            break;
        }

        write(new_sock, buff, sizeof(buff));
        memset(buff, 0, sizeof(buff));
        read(new_sock, buff, sizeof(buff));
        printf("Received from server : %s", buff);
    }

    close(new_sock);
    return 0;
}
