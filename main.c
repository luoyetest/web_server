#include <stdio.h>
#include "http.h"

#define HTTP_COM_START "start"
#define HTTP_COM_STOP "stop"
#define HTTP_COM_QUIT "quit"

int main(int argc, char *argv[]){
	char com[10];
	int serve = 0;
	pthread_t http_listen, http_server;
	while(1){
		gets(com);
		if(cmp(com, HTTP_COM_START)){
			if(serve){
				printf("Can not start http server again,\n");
				continue;
			}
			if(!http_init()){
				perror("http_init:error-->");
				continue;
			}
			pthread_create(&http_listen, NULL, (void *)http_start, NULL);
			pthread_create(&http_server, NULL, (void *)http_serve, NULL);
			serve = 1;
		}
		else if(cmp(com, HTTP_COM_STOP)){
			pthread_cancel(http_server);
			pthread_cancel(http_listen);
  			pthread_join(http_server,NULL);
  			pthread_join(http_listen,NULL);
  			serve = 0;
			http_stop();
			break;
		}
		else{
			printf("illegal command!\n");
			continue;
		}
	}
	return 0;
}
