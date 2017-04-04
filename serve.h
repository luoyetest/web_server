#ifndef SERVE
#define SERVE

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>

int serve_init(int port);
void serve_start(int server_sockfd, void (* func)(int));
void serve_close(int server_sockfd);

#endif
