#include "socketutils.h"
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>


struct AcceptedSocket{
    int acceptedSocketFD;
    struct sockaddr_in address;
    int error;
    bool acceptedSuccessfully;
};

struct AcceptedSocket *acceptIncomingConnection(int serverSocketFD);

void *recvPrintData(void *arg);
// void recvPrintData(struct AcceptedSocket *clientSocket);
void recvPrintDataThread(struct AcceptedSocket *socket);

struct AcceptedSocket acceptedSockets[10];
int acceptedSocketCount = 0;
void sendMessageToOthers(char *buffer, int socketFD);

int main()
{
    int serverSocketFD = createTCPIPv4Socket();
    struct sockaddr_in* serverAddress = createIPv4Address("", 2000);
    
    int result = bind(serverSocketFD, (const struct sockaddr *) serverAddress, sizeof(*serverAddress));
    printf("Binding result: %d\n", result);
    if (result == 0){
        printf("Binding successfully\n");
    }

    int listenResult = listen(serverSocketFD, 10);
    // printf("Binding result: %d\n", result);

    // struct sockaddr_in clientAddress;
    // socklen_t clientAddressSize = sizeof(struct sockaddr_in);
    // int clientSocketFD = accept(serverSocketFD, (struct sockaddr *)&clientAddress, &clientAddressSize);
    while (true)
    {

        struct AcceptedSocket *clientSocket = acceptIncomingConnection(serverSocketFD);
        acceptedSockets[acceptedSocketCount++] = *clientSocket;
        // pthread_t id;
        // pthread_create(&id, NULL, recvPrintDataThread, clientSocket);
        // recvPrintData(clientSocket);
        recvPrintDataThread(clientSocket);
    }



    // close(clientSocket->acceptedSocketFD);
    close(serverSocketFD);
    free(serverAddress);


    shutdown(serverSocketFD, SHUT_RDWR);
    printf("Socket closed.\n");

    
    return 0;
}

void recvPrintDataThread(struct AcceptedSocket *socket){
    pthread_t id;
    pthread_create(&id, NULL, recvPrintData, socket);
}

void *recvPrintData(void *arg)
// void recvPrintData(struct AcceptedSocket *clientSocket)
{
    struct AcceptedSocket *clientSocket = (struct AcceptedSocket *)arg;
    int socketFD = clientSocket->acceptedSocketFD; // Extract file descriptor
    char buffer[1024];

    while (true)
    {
        ssize_t amountReceived = recv(socketFD, buffer, 1024, 0);
        if (amountReceived > 0)
        {
            buffer[amountReceived] = '\0';
            printf("%s\n", buffer);
            // printf("Response from server:\n%s\n", buffer);
            sendMessageToOthers(buffer, socketFD);
            // printf("broadcasted message to others\n");
        }
        else
        {
            printf("Error receiving data\n");
            break;
        }
    }
    return NULL;
}

void sendMessageToOthers(char *buffer, int socketFD){
    for (int i=0; i<acceptedSocketCount; i++){
        if (acceptedSockets[i].acceptedSocketFD != socketFD){
            send(acceptedSockets[i].acceptedSocketFD, buffer, strlen(buffer),0);
        }
    }
};

struct AcceptedSocket *acceptIncomingConnection(int serverSocketFD){
    struct sockaddr_in clientAddress;
    socklen_t clientAddressSize = sizeof(struct sockaddr_in);
    int clientSocketFD = accept(serverSocketFD, (struct sockaddr *)&clientAddress, &clientAddressSize);
    struct AcceptedSocket *acceptedSocket = malloc(sizeof(struct AcceptedSocket));
    acceptedSocket->address = clientAddress;
    acceptedSocket->acceptedSocketFD = clientSocketFD;
    acceptedSocket->acceptedSuccessfully = clientSocketFD > 0;
    // Print original (raw) values
    // printf("Accepted socket address (raw):\n");
    // printf("  IP (hex): 0x%08x\n", clientAddress.sin_addr.s_addr);
    // printf("  IP (octets): %d.%d.%d.%d\n",
    //        (clientAddress.sin_addr.s_addr >> 0) & 0xFF,
    //        (clientAddress.sin_addr.s_addr >> 8) & 0xFF,
    //        (clientAddress.sin_addr.s_addr >> 16) & 0xFF,
    //        (clientAddress.sin_addr.s_addr >> 24) & 0xFF);
    // printf("  IP (octets): %d.%d.%d.%d\n",
    //        (clientAddress.sin_addr.s_addr >> 24) & 0xFF, // First octet
    //        (clientAddress.sin_addr.s_addr >> 16) & 0xFF, // Second octet
    //        (clientAddress.sin_addr.s_addr >> 8) & 0xFF,  // Third octet
    //        (clientAddress.sin_addr.s_addr >> 0) & 0xFF); // Fourth octet
    // printf("  Port (hex): 0x%04x\n", clientAddress.sin_port);
    // printf("  Port (decimal): %u\n", clientAddress.sin_port);
    printf("Accepted socket FD: %d\n", clientSocketFD);
    printf("Accepted socket address: %s:%d\n", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));
    if (!acceptedSocket->acceptedSuccessfully) {
        acceptedSocket->error = clientSocketFD;
    }

    return acceptedSocket;
};