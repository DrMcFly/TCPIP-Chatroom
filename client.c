#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define IP "192.168.50.17"
#define PORT 2005

// uint8_t *DEVICE_ID = 0001;

int main() {
    char *buffer = (char *)malloc(4026);

    struct sockaddr_in server;
    int lfd;

    // Create socket
    lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Set up server address
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);  // Change port to 2006
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to server
    if (connect(lfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("connect");
        close(lfd);
        exit(EXIT_FAILURE);
    }

    int numchars = 15;
    char name[] = "Martin";
    // printf("%ld", strlen(name));
    size_t data_length = (strlen(name) > numchars) ? numchars : strlen(name);
    memmove(buffer, name, data_length);

    char lname[] = "McCorkle";
    data_length = (strlen(lname) > numchars) ? numchars : strlen(lname);
    memmove(buffer + numchars, lname, data_length);
    
    // Print the first part (up to the first name)
    printf("First name in buffer: %.*s\n", (int)strlen(name), buffer);

    // Print the second part (starting from where the last name is inserted)
    printf("Last name in buffer: %.*s\n", (int)strlen(lname), buffer + numchars);

    printf("Attempting send...");
    send(lfd, buffer, 4026, 0);
    free(buffer);
    printf("Sent!\n");

    gets();

    send(lfd, "", 0, 0);
    close(lfd);

    return 0;
}