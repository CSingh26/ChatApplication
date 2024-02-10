#include "network.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int setupServer(int port, int backlog) {
    int serverFd;
    struct sockaddr_in serverAddr;

    serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd == -1) {
        perror("Could not create socket");
        return -1;
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind failed");
        return -1;
    }

    if (listen(serverFd, backlog) < 0) {
        perror("Listen failed");
        return -1;
    }

    return serverFd;
}

int acceptClient(int serverFd) {
    struct sockaddr_in clientAddr;
    socklen_t addrSize = sizeof(clientAddr);
    int clientFd = accept(serverFd, (struct sockaddr *)&clientAddr, &addrSize);
    if (clientFd < 0) {
        perror("Accept failed");
        return -1;
    }
    return clientFd;
}

int connectToServer(const char *serverIp, int port) {
    int sock;
    struct sockaddr_in serverAddr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Could not create socket");
        return -1;
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);

    if (inet_pton(AF_INET, serverIp, &serverAddr.sin_addr) <= 0) {
        perror("Invalid address / Address not supported");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Connect failed");
        return -1;
    }

    return sock;
}

// A simple main function for demonstration
int main() {
    int port = 8080;
    int serverFd = setupServer(port, 10);
    if (serverFd == -1) {
        fprintf(stderr, "Failed to set up server on port %d\n", port);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", port);

    // Accept a client connection just for demonstration
    int clientFd = acceptClient(serverFd);
    if (clientFd == -1) {
        fprintf(stderr, "Failed to accept client\n");
        close(serverFd);
        exit(EXIT_FAILURE);
    }

    printf("Client connected\n");

    close(clientFd);
    close(serverFd);

    return 0;
}
