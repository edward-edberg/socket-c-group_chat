// #include <arpa/inet.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <errno.h>
// #include <stdio.h>
// #include <unistd.h>
// #include <stdlib.h>
// #include <string.h>


// int createTCPIPv4Socket() {
//     int socketFD = socket(AF_INET, SOCK_STREAM, 0);
//     if (socketFD == -1) {
//         perror("Socket creation failed");
//         return -1;
//     }
//     return socketFD;
// }

// struct sockaddr_in* createIPv4Address(const char* ip, int port) {
//     struct sockaddr_in* address = malloc(sizeof(struct sockaddr_in));
//     address->sin_family = AF_INET;
//     address->sin_port = htons(port);
    
//     // Convert IP address from string to binary form
//     if (inet_pton(AF_INET, ip, &address->sin_addr.s_addr) <= 0) {
//         perror("Invalid IP address/Address not supported");
//         address->sin_addr.s_addr = INADDR_NONE; // Set to an invalid address
//     }
    
//     return address;
// }

#include "socketutils.h"
#include <stdbool.h>

#include <pthread.h>

void startListenThread(int socketFD);

void* listenForData(void *arg);


int main(){

    int socketFD = createTCPIPv4Socket();

    struct sockaddr_in* address = createIPv4Address("127.0.0.1",2000);



    // printf("Attempting to connect to %s:%d...\n", ip, ntohs(address.sin_port));

    // int connect_result = connect(socketFD, (const struct sockaddr *) &address, sizeof(address));
    int connect_result = connect(socketFD, (const struct sockaddr *) address, sizeof(*address));
    // print()
    if (connect_result == -1){
        perror("Connection failed"); // perror gives more descriptive error messages
        close(socketFD);
        return 1;
    } else {
        printf("Connection successful\n");
    }
    // --- End Connect Functionality ---
    startListenThread(socketFD);

    // add name and print it

    char *name = NULL;
    size_t nameSize = 0;
    printf("Enter your name:");
    ssize_t nameCount = getline(&name, &nameSize, stdin);
    name[nameCount-1] = 0;

    char *line = NULL;
    size_t lineSize = 0;
    printf("Type something and we will send it to the server. Type exit to close the program~\n");


    char buffer[1024];
    while(true){
        ssize_t charCount = getline(&line, &lineSize,stdin);
        line[charCount-1] = 0;

        sprintf(buffer, "%s:%s", name, line);
        // printf("Sending: %s", buffer);
        if (charCount > 0){
            if (strcmp(line, "exit\n") == 0){
                break;
            }
            ssize_t amountWasSent = send(socketFD, buffer, strlen(buffer), 0);

        }
    }

    // char* message ;
    // message = "GET / HTTP/1.1\r\nHost: google.com\r\nConnection: close\r\n\r\n";
    // send(socketFD, message, strlen(message), 0);

    // char buffer[1024];
    // recv(socketFD, buffer, 1024, 0);

    // printf("Response from server:\n%s\n", buffer);
    close(socketFD);
    printf("Socket closed.\n");
    return 0;
}

void startListenThread(int socketFD){
    pthread_t id;
    int *socketPtr = malloc(sizeof(int));
    *socketPtr = socketFD;
    pthread_create(&id, NULL, listenForData, socketPtr);
}

void* listenForData(void *arg){
    printf("Listening for data...\n");
    // int socketFD;
    int socketFD = *(int *)arg;
    char buffer[1024];

    while(true)
    {
        ssize_t amountReceived = recv(socketFD, buffer, 1024, 0);

        if (amountReceived > 0)
        {
            buffer[amountReceived] = '\0';
            printf("Response from server:\n%s\n", buffer);
        }
        else
        {
            printf("Error receiving data\n");
            break;
        }
    }
    close(socketFD);
    return NULL;
}