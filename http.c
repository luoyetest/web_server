#include "http.h" 

int http_init(){
	non_keeping = (struct no_keep *)malloc(sizeof(struct no_keep));
	if(non_keeping == NULL ){
		return 0;
	}
	non_keeping->in = 0;
	non_keeping->out = 0;
	return 1;
}

void *http_start(){
	int  conn_fd; 
	char buf[1000];
	int len;
	//pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	//int server_sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	struct sockaddr_in server_sockaddr;
	server_sockaddr.sin_family = AF_INET;
	server_sockaddr.sin_port = htons(HTTP_PORT);
	server_sockaddr.sin_addr.s_addr = INADDR_ANY;
	
	if(bind(server_sockfd,(struct sockaddr *)&server_sockaddr,sizeof(server_sockaddr))==-1){
		perror("serve_init:bind error-->");
		return;
	}
	if(listen(server_sockfd,120) == -1){
		perror("serve_init:listen error-->");
		return;
	}
	printf("The service has been built at port(%d)\n", HTTP_PORT);
	//struct sockaddr_in client_addr;
	socklen_t length = sizeof(struct sockaddr_in);
    
	while(1){
		if((conn_fd = accept(server_sockfd, (struct sockaddr *)&non_keeping->sock[non_keeping->in], &length)) == -1){  
			perror("http_start:accept error.");  
			continue;  
		}  
		non_keeping->sockfd[non_keeping->in] = conn_fd; 
		non_keeping->in = (++non_keeping->in)%QUE_SIZE;
	}
}

void http_stop(){
	close(server_sockfd);
	//free memory
	free(non_keeping);
	printf("The service has been stopped.\n");
}

void *http_serve(){
	char *buf;
	long int len;
	int nk;
	//pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	buf = (char *)malloc(sizeof(char)*BUF_SIZE);
	if(buf == NULL){
		perror("http_serve:run out of memeroy-->");  
		//exit(0);
	}
	while(1){
		if(non_keeping->in == non_keeping->out){
			continue;
		}
		nk = http_recv();
		if(nk == 0){
			continue;
		}
		if(!http_send()){
			http_send();
		}
		free((non_keeping->message[non_keeping->out]).url);
		//free((non_keeping->message[non_keeping->out]).data);
		close(non_keeping->sockfd[non_keeping->out]);
		non_keeping->out = (++non_keeping->out)%QUE_SIZE;
	}
}

int http_recv(){
	int len = 0;
	int nk = 1;
	char buf[BUF_SIZE];
	len = recv(non_keeping->sockfd[non_keeping->out], &buf, BUF_SIZE, 0);
	if(len <= 0){
		return 0;
	}
	get_message(buf);
	if(len == BUF_SIZE){
		//next read
	}
	return nk;
}

int get_message(char *buf){
	int i = 0;
	int j = 0;
	int data_flag = 0;
	int nk = 1;
	char temp[1024];
	for(i=0; i<BUF_SIZE; i++){
		if(buf[i] == ' '){
			(non_keeping->message[non_keeping->out]).method[j] = '\0';
			i++;
			break;
		}
		(non_keeping->message[non_keeping->out]).method[j++] = buf[i];
	}
	for(j=0; i<BUF_SIZE; i++){
		if(buf[i] == ' '){
			temp[j] = '\0';
			(non_keeping->message[non_keeping->out]).url = (char *)malloc(sizeof(char)*j);
			strcpy((non_keeping->message[non_keeping->out]).url, temp);
			i++;
			break;
		}
		temp[j++] = buf[i]; 
	}
	for(j=0; i<BUF_SIZE; i++){
		if(buf[i] == '\n'){
			(non_keeping->message[non_keeping->out]).agreement[j] = '\0';
			i++;
			break;
		}
		(non_keeping->message[non_keeping->out]).agreement[j++] = buf[i];
	}
	//read head line
	/*
	while(data_flag < 2){
		
	}
	*/
	return nk;
}

int http_send(){
	char index[2] = "\\0";
	char file[1000];
	char *buf;
	//char *p;
	int len = 0;
	if(cmp((non_keeping->message[non_keeping->out]).url, index)){
		strcpy(file,"./index.html");
	}
	else{
		file[0] = '.';
		brige(file, (non_keeping->message[non_keeping->out]).url, 1);\
	}
	/*
	else{
		strcpy(file,".");
		strcat(file,(non_keeping->message[non_keeping->out]).url);
	}
	*/
	
	printf("%s\n",file);
	buf = joint_message(file, &len);
	if(buf == NULL){
		return 0;
	}
	send(non_keeping->sockfd[non_keeping->out], buf, len, 0);
	free(buf);
	return 1;
}

char *joint_message(char *file, int *size){
	FILE *fp;
	char *buf;
	int len = 0, count;
	char temp[1024];
	count = get_file_size(file);
	buf = malloc(count + 500);
	buf[0] = '\0';
	if(count < 0){
		len = brige(buf, NOT_404, len);
		strcpy(file,"./404.html");
	}
	else{
		len = brige(buf, OK_200, len);
	}
	fp = fopen(file, "r");
	len = brige(buf, CONNECTION, len);
	len = brige(buf, SERVER, len);
	len = brige(buf, CONTENT_TYPE, len);
	//data
	len = brige(buf, "\n", len);
	while(!feof(fp)){
		count = (int)fread(temp, sizeof(char), 1023, fp);
		temp[++count] = '\0';
		len = brige(buf, temp, len);
	}
	fclose(fp);

	*size = len-1;
	return buf;
}

unsigned int get_file_size(const char *path){
	unsigned int filesize = -1;
	struct stat statbuff;
	if(stat(path, &statbuff) < 0){  
		return filesize;  
	}
	else{  
		filesize = statbuff.st_size;  
	}
	return filesize;  
}

int brige(char *f, char *s, int i){
	int j=0;
	while(s[j]!='\0'){
		f[i++] = s[j++];
	}
	f[i] = '\0';
	return i;
}

int cmp(char *f, char *s){
	while(*f != '\0' && *f++ == *s++);
	if(*f != '\0')
		return 0;
	return 1;
}
