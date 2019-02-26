#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include "include/BAL.h"
#define MSG_LENGTH 30

BAL_user *headUser;

int main(int argc, char **argv){
    
	headUser=malloc(sizeof(BAL_user));
	
	int sockListen;
	struct hostent *hp;
	struct sockaddr_in adr_distant;
	struct sockaddr_in adr_local;
	char *msg;
	msg=malloc(sizeof(char)*MSG_LENGTH);
	
	fd_set rfds;
	 struct timeval tv;
	 int retval ;
	tv.tv_sec = 5;
 	tv.tv_usec = 0;

	
	sockListen=socket(AF_INET,SOCK_STREAM,0);
	if(sockListen==-1){
		printf("Failed creating sockListen");
		exit(1);
	}
	memset((char*)& adr_local,0,sizeof(adr_local));
	adr_local.sin_family=AF_INET;
	adr_local.sin_port=htons(atoi(argv[argc-1]));
	adr_local.sin_addr.s_addr=INADDR_ANY;
	
	//bind 
	if (bind(sockListen,(struct sockaddr*)&adr_local,sizeof(adr_local))==-1){
		printf("fail bind\r\n");
		exit(1);
	}
	
	if(listen(sockListen,1)==-1){
		printf("Failed listen\r\n");
		exit(1);
	}
	
	printf("Server BAL started !\r\n");
	
	//accept
	int addrsize=sizeof(adr_distant);
	int sockClient=accept(sockListen,(struct sockaddr*)&adr_distant,(socklen_t*)&addrsize);
	if(sockClient==-1){
		printf("Failed accept\r\n");
	}
	//read
	int ind_msg=0;
    int l=1;
	while(l!=0){
		l=read(sockClient,msg,MSG_LENGTH);
		if(l==-1){
			printf("Failed Read\r\n");
			exit(1);
		}
		//printf("l=%d\r\n",l);
		if(l!=0){
			int id;
			char *content=malloc((l-5)*sizeof(char));
			sscanf(msg,"%d%s",&id,content);
			printf("Reception n°%d (%d) [%s] a destination de %d\r\n",++ind_msg,l,msg,id);
			storeMsg(headUser,id,content);
		}
	}
	printf("exited read loop\r\n");
	//shutdown
	if(shutdown(sockClient,2)==-1){
		printf("Failed shutdown sockClient\r\n");
		exit(1);
	}
	//close
	if(close(sockClient)==-1){
		printf("failed destructing sockClient\r\n");
		exit(1);
	}
	//destruct socket (commun T/R)
	if(close(sockListen)==-1){
		printf("failed destructing socket\r\n");
		exit(1);
	}
	
	printf("Hello world !\r\n");
	return 0;
}
