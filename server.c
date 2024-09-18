#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>

#define IP "192.168.50.17"
#define PORT 2005
#define BACKLOG 5

int main() {
    struct sockaddr_in client, server;
    int lfd, confd;
    int reuse = 1;
    char r_buff[4026] = {};

    // Create a socket
    lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Set socket options
    if (setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("setsockopt");
        close(lfd);
        exit(EXIT_FAILURE);
    }

    // Prepare server address
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");  // Accept connections from any interface

    // Bind the socket
    if (bind(lfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("bind");
        close(lfd);
        exit(EXIT_FAILURE);
    }

    printf("Socket bound to port %d\n", PORT);

    while(1) {
        // Listen for incoming connections
        if (listen(lfd, BACKLOG) < 0) {
            perror("listen");
            close(lfd);
            exit(EXIT_FAILURE);
        }

        // Accept incoming connection
        socklen_t client_len = sizeof(client);
        confd = accept(lfd, (struct sockaddr *)&client, &client_len);
        if (confd < 0) {
            perror("accept");
            close(lfd);
            exit(EXIT_FAILURE);
        }

        // Reset buffer memory;   
        memset(r_buff, 0, 4025);

        // Recieve data and process buffer
        int bytes = recv(confd, &r_buff, 4025, 0);
        
        // Figure out how to get this working.
        //
        // if (bytes == 0) {
        //     printf("Connection closed by client\n");
        //     close(confd);
        //     close(lfd);
        // }

        if (bytes < 0) {
            perror("recv");
            close(lfd);
            exit(EXIT_FAILURE);
        } else {
            char name[15 + 1];
            char lastinit;
            lastinit = r_buff[15];
            
            memcpy(name, r_buff, 15);
            name[15] = '\0';

            printf("\n[ %s %c. ]: \"", name, lastinit);
            for (int i = 0; i < 30; i++) {
                if (r_buff[i] == '\0') {
                    printf(" ");
                }
                printf("%c", (uint8_t)r_buff[i]);
            }
            printf("\"\n");
        }    
    }
    
    // Close the connected socket and the listening socket
    close(confd);
    close(lfd);

    return 0;
}