#include "socketutils.h"


int createTCPIPv4Socket() {
    int socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFD == -1) {
        perror("Socket creation failed");
        return -1;
    }
    return socketFD;
}

struct sockaddr_in* createIPv4Address(const char* ip, int port) {
    struct sockaddr_in* address = malloc(sizeof(struct sockaddr_in));
    address->sin_family = AF_INET;
    address->sin_port = htons(port);

    if (strlen(ip) == 0){
        address->sin_addr.s_addr = INADDR_ANY;
    } else {
        // Convert IP address from string to binary form
        if (inet_pton(AF_INET, ip, &address->sin_addr.s_addr) <= 0) {
            perror("Invalid IP address/Address not supported");
            address->sin_addr.s_addr = INADDR_NONE; // Set to an invalid address
        }
        

    }
    
    return address;
}