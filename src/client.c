#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define SERVER_PORT 8080
#define BUFFER_SIZE 1024

volatile int clientActive = 1;

void* receiveMessages(void* socketDesc) {
    int sock = *(int*)socketDesc;
    char serverReply[BUFFER_SIZE];
    int readSize;

    while(clientActive && (readSize = recv(sock, serverReply, sizeof(serverReply), 0)) > 0) {
        serverReply[readSize] = '\0';
        printf("Server reply: %s\n", serverReply);
        memset(serverReply, 0, sizeof(serverReply));
    }

    if(readSize == 0) {
        puts("Server disconnected.");
        clientActive = 0;
    } else if(readSize == -1) {
        perror("recv failed");
    }

    return 0;
}

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in server;
    char message[BUFFER_SIZE];
    pthread_t receiveThread;

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Could not create socket");
    }
    puts("Socket created");

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("connect failed");
        return 1;
    }

    puts("Connected to server\n");

    if(pthread_create(&receiveThread, NULL, receiveMessages, (void*)&sock) < 0) {
        perror("could not create thread");
        return 1;
    }

    printf("Enter message: ");
    while(fgets(message, sizeof(message), stdin) != NULL) {
        if(send(sock, message, strlen(message), 0) < 0) {
            perror("send failed");
            return 1;
        }
        printf("Enter message: ");
    }

    clientActive = 0;
    pthread_join(receiveThread, NULL);
    close(sock);

    return 0;
}
