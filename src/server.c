#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>

#define PORT 8080
#define MAX_CLIENTS 10

int clientSockets[MAX_CLIENTS];
pthread_mutex_t clientSocketsMutex = PTHREAD_MUTEX_INITIALIZER;

void broadcastMessage(int senderSocket, char *message) {
    pthread_mutex_lock(&clientSocketsMutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clientSockets[i] != 0 && clientSockets[i] != senderSocket) {
            send(clientSockets[i], message, strlen(message), 0);
        }
    }
    pthread_mutex_unlock(&clientSocketsMutex);
}

void* handleClient(void* arg) {
    int sock = *(int*)arg;
    char buffer[1024];
    int readSize;

    while ((readSize = recv(sock, buffer, sizeof(buffer), 0)) > 0) {
        buffer[readSize] = '\0';
        broadcastMessage(sock, buffer);
    }

    if (readSize == 0) {
        puts("Client disconnected");
        fflush(stdout);
    } else if (readSize == -1) {
        perror("recv failed");
    }

    pthread_mutex_lock(&clientSocketsMutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clientSockets[i] == sock) {
            clientSockets[i] = 0;
            break;
        }
    }

    pthread_mutex_unlock(&clientSocketsMutex);
    close(sock);

    return 0;
}

int main() {
    int serverFd, clientSock, c, *newSock;
    struct sockaddr_in server, client;

    memset(clientSockets, 0, sizeof(clientSockets));

    serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd == -1) {
        perror("Could not create socket");
        return 1;
    }
    puts("Socket created");

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    if (bind(serverFd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Bind failed");
        return 1;
    }
    puts("Bind done");

    listen(serverFd, 3);

    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    while ((clientSock = accept(serverFd, (struct sockaddr *)&client, (socklen_t*)&c))) {
        puts("Connection accepted");

        pthread_t client_thread;
        newSock = malloc(1);
        *newSock = clientSock;

        // Add client socket to array
        pthread_mutex_lock(&clientSocketsMutex);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clientSockets[i] == 0) {
                clientSockets[i] = clientSock;
                break;
            }
        }
        pthread_mutex_unlock(&clientSocketsMutex);

        if (pthread_create(&client_thread, NULL, handleClient, (void*)newSock) < 0) {
            perror("Could not create thread");
            return 1;
        }
        puts("Handler assigned");
    }

    if (clientSock < 0) {
        perror("Accept failed");
        return 1;
    }
    return 0;
}