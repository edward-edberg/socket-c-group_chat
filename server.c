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
            printf("Response from server:\n%s\n", buffer);
        }
        else
        {
            printf("Error receiving data\n");
            break;
        }
    }
    return NULL;
}

struct AcceptedSocket *acceptIncomingConnection(int serverSocketFD){
    struct sockaddr_in clientAddress;
    socklen_t clientAddressSize = sizeof(struct sockaddr_in);
    int clientSocketFD = accept(serverSocketFD, (struct sockaddr *)&clientAddress, &clientAddressSize);
    struct AcceptedSocket *acceptedSocket = malloc(sizeof(struct AcceptedSocket));
    acceptedSocket->address = clientAddress;
    acceptedSocket->acceptedSocketFD = clientSocketFD;
    acceptedSocket->acceptedSuccessfully = clientSocketFD > 0;

    if (!acceptedSocket->acceptedSuccessfully) {
        acceptedSocket->error = clientSocketFD;
    }

    return acceptedSocket;
};