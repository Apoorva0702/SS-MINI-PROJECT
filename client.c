#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/select.h>

int main(int argc, char *argv[]) {
    if(argc != 3) {
        printf("Usage: %s <server-ip> <port>\n", argv[0]);
        return 1;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server;

    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &server.sin_addr);

    if(connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        printf(" Connection Failed.\n");
        return 1;
    }

    printf(" Connected to server.\n\n");

    fd_set set;
    char buf[1024];

    while(1) {
        FD_ZERO(&set);
        FD_SET(0, &set);
        FD_SET(sock, &set);

        select(sock+1, &set, NULL, NULL, NULL);

        if(FD_ISSET(0, &set)) {   // keyboard input → send to server
            int n = read(0, buf, sizeof(buf));
            if(n <= 0) break;
            write(sock, buf, n);
        }

        if(FD_ISSET(sock, &set)) { // server output → print to screen
            int n = read(sock, buf, sizeof(buf));
            if(n <= 0) break;
            write(1, buf, n);
        }
    }

    close(sock);
    return 0;
}
