// network.h
#ifndef NETWORK_H
#define NETWORK_H

#include <sys/socket.h>
#include <netinet/in.h>

int setupServer(int port, int backlog);
int acceptClient(int serverFd);
int connectToServer(const char *serverIp, int port);

#endif // NETWORK_H
