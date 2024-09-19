#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define IP "127.0.0.1"
#define PORT 2006

int main() {
    char *buffer = (char *)malloc(4026);

    struct sockaddr_in server;
    int lfd;

    // Connect to server
    // if (connect(lfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
    //     perror("connect");
    //     close(lfd);
    //     exit(EXIT_FAILURE);
    // }

    system("clear");

    int numchars = 15;
    char name[] = "First";
    // printf("%ld", strlen(name));
    size_t data_length = (strlen(name) > numchars) ? numchars : strlen(name);
    memmove(buffer + 1, name, data_length);

    char lname[] = "Last";
    data_length = (strlen(lname) > numchars) ? numchars : strlen(lname);
    memmove(buffer + numchars + 1, lname, data_length);

    char *user_flag = "1";
    memmove(buffer, user_flag, 1);

    // Print the first part (up to the first name)
    // printf("First name in buffer: %.*s\n", (int)strlen(name), buffer);

    // Print the second part (starting from where the last name is inserted)
    // printf("Last name in buffer: %.*s\n", (int)strlen(lname), buffer + numchars);

    int m_length = 500;

    while(1) {
        
        // Create socket
        lfd = socket(AF_INET, SOCK_STREAM, 0);
        if (lfd < 0) {
            perror("socket");
            exit(EXIT_FAILURE);
        }

        // Set up server address
        server.sin_family = AF_INET;
        server.sin_port = htons(PORT);  // Change port to 2006
        server.sin_addr.s_addr = inet_addr(IP);

        if (connect(lfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
            perror("connect");
            close(lfd);
            exit(EXIT_FAILURE);
        }
        
        memset(buffer + 31, 0, m_length);
        char message[m_length];

        fgets(message, sizeof(message), stdin);

        data_length = (strlen(message) > m_length) ? m_length : strlen(message);
        memmove(buffer + 30, message, data_length);

        // printf("Buffer contents (%zu bytes):\n", data_length);
        
        char *keyword = ":exit";
        if (strstr(buffer + 30, keyword) != NULL) {
            printf("Closing connection...\n");
            close(lfd);
            system("clear");
            return 0;
        }

        send(lfd, buffer, 4026, 0);

        system("clear");

        // close(lfd);

    }

    free(buffer);
    gets();

    // send(lfd, "", 0, 0);
    close(lfd);

    return 0;
}