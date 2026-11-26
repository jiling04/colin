#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERVER_DEFAULT_IP   "192.168.2.168"
#define SERVER_DEFAULT_PORT 1500
#define MAX_CLIENTS         1000

#define _IP_   SERVER_DEFAULT_IP
#define _PORT_ SERVER_DEFAULT_PORT

int net_init(const char *ip, int port);
int first_response(int sock, const char *cip);
int selectModel(const char *ip, int port);

#endif
