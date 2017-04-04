#include <stdio.h>

void aaa(int i){
	printf("%d",i);
}

void sss(int server_sockfd, void (* func)(int)){
	(*func)(server_sockfd);
}

int main(){
	sss(1, aaa);
} 