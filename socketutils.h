#ifndef SOCKETUTILS_H
#define SOCKETUTILS_H


#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


int createTCPIPv4Socket();

struct sockaddr_in* createIPv4Address(const char* ip, int port);

#endif
