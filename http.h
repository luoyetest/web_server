#ifndef HTTP
#define HTTP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/stat.h> 

#define HTTP_PORT 80
#define QUE_SIZE 100
#define BUF_SIZE 1024*2

struct http{
	char method[10];
	char *url;
	char agreement[10];
	char host[16];
	char connection[15];
	char *data;
};

struct no_keep{
	int in;
	int out;
	int sockfd[QUE_SIZE];
	struct sockaddr_in sock[QUE_SIZE];
	struct http message[QUE_SIZE];
};

struct keep{
	int in;
	int out;
	int sockfd[QUE_SIZE];
	struct sockaddr_in sock[QUE_SIZE];
	struct http message[QUE_SIZE];
};

struct no_keep *non_keeping;
struct keep *keeping;

void http_start();
const int http_init();
void http_stop();
int http_send();
int http_recv();
void *deal_keeping();
void *http_serve();
int get_message(char *buf);
char *joint_message(char *file, int *size);
void change_que();
unsigned int get_file_size(const char *path);
int brige(char *f, char *s, int i);
int cmp(char *f, char *s);



#endif
