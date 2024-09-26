#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#define IP "127.0.0.1"
#define PORT 2006
#define BACKLOG 10

#define PACKET_SIZE 4026

int socket_init(); // Initializes Socket Variables
int accept_conn(); // Listens and Accepts Connections
int receive_packet(); // Receives Packets
int print_time(); // Prints Formatted Time
int socket_close(); // Closes Socket FDs

// Global socket variables
struct sockaddr_in client, server;
const int reuse = 1;
int local_fd, connection_fd;
char receive_buffer[PACKET_SIZE] = {};
char send_buffer[PACKET_SIZE] = {};

// Global multithreading variables
pthread_t thread;

int main() {
    
    socket_init();

    system("clear");

    while (1) {
        
        pthread_create(&thread, NULL, accept_conn, NULL);
        // accept_conn();
        pthread_join(thread, NULL);
        receive_packet();
        
    }
    
    socket_close();
    
    return 0;
}

int print_time() {
    time_t current_time;
    char time_string[20];
    struct tm *tm_info;
    
    current_time = time(NULL);

    tm_info = localtime(&current_time);
    if (tm_info == NULL) {
        perror("localtime");
        return -1;
    }

    snprintf(time_string, sizeof(time_string),
             "%02d/%02d/%04d-%02d:%02d:%02d",
             tm_info->tm_mon + 1,  // Month (1-12)
             tm_info->tm_mday,     // Day of month (1-31)
             tm_info->tm_year + 1900, // Year (e.g., 2024)
             tm_info->tm_hour,     // Hours (0-23)
             tm_info->tm_min,      // Minutes (0-59)
             tm_info->tm_sec);     // Seconds (0-59)
    printf("%s", time_string);

    return 0;
}

int receive_packet() {
    // Reset buffer memory;   
    memset(receive_buffer, 0, PACKET_SIZE);

    // Recieve data and record time
    int bytes = recv(connection_fd, &receive_buffer, sizeof(receive_buffer), 0);
    if (receive_buffer[0] == '1') {
        char name[15 + 1];
        char lastinit;
        lastinit = receive_buffer[16];
        
        memcpy(name, receive_buffer + 1, 15);
        name[15] = '\0';
        // printf("%d", bytes);
        
        // print username and time
        print_time();
        printf(" [%s %c]: ", name, lastinit);
        
        // print message
        for (int i = 30; i < 500 && receive_buffer[i] != '\0'; i++) {
            if (receive_buffer[i] == '\n') {
                receive_buffer[i] = '\0';
            }
            printf("%c", (uint8_t)receive_buffer[i]);
        }
        printf("\n");
    } else if (bytes == 0) {
        print_time();
        printf(" \e[32m[System]\e[0m: Connection closed by client\n");
        close(connection_fd);
    } else if (bytes < 0) {
        perror("recv");
        close(local_fd);
        exit(EXIT_FAILURE);
    }  
}

int socket_init() {
    // Create a socket
    local_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (local_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Set socket options
    if (setsockopt(local_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("setsockopt");
        close(local_fd);
        exit(EXIT_FAILURE);
    }

    int keepalive = 1;
    setsockopt(local_fd, SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(keepalive));

    // Prepare server address
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr(IP);

    // Bind the socket
    if (bind(local_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("bind");
        close(local_fd);
        exit(EXIT_FAILURE);
    }
    printf("Socket bound to port %d\n", PORT);

}

int accept_conn() {
    // Listen for incoming connections
    if (listen(local_fd, BACKLOG) < 0) {
        perror("listen");
        close(local_fd);
        exit(EXIT_FAILURE);
    }

    // Accept incoming connection
    socklen_t client_len = sizeof(client);
    connection_fd = accept(local_fd, (struct sockaddr *)&client, &client_len);
    if (connection_fd < 0) {
        perror("accept");
        close(local_fd);
        exit(EXIT_FAILURE);
    }
}

int socket_close() {
    // Close the connected socket and the listening socket
    close(connection_fd);
    close(local_fd);
}