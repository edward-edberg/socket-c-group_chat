#include "socketutils.h"
#include <stdio.h>
#include <stdbool.h>

int main() {
    int serverSocketFD = createTCPIPv4Socket();
    struct sockaddr_in* serverAddress = createIPv4Address("", 2000);
    
    int result = bind(serverSocketFD, (const struct sockaddr *) serverAddress, sizeof(*serverAddress));
    printf("Binding result: %d\n", result);
    if (result == 0){
        printf("Binding successfully\n");
    }

    int listenResult = listen(serverSocketFD, 10);
    // printf("Binding result: %d\n", result);

    struct sockaddr_in clientAddress;
    socklen_t clientAddressSize = sizeof(struct sockaddr_in);
    int clientSocketFD = accept(serverSocketFD, (struct sockaddr *)&clientAddress, &clientAddressSize);
    char buffer[1024];

    while(true){
        ssize_t amountReceived = recv(clientSocketFD, buffer, 1024, 0);
        if (amountReceived > 0){
            buffer[amountReceived] = '\0';
            printf("Response from server:\n%s\n", buffer);
        }
        else {
            printf("Error receiving data\n");
            break;
        }

    }

    close(clientSocketFD);
    shutdown(serverSocketFD, SHUT_RDWR);
    printf("Socket closed.\n");

    
    return 0;
}