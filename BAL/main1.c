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
    struct sockaddr_in adr_distant;
    struct sockaddr_in adr_local;
    
    char *msg;
    msg=malloc(sizeof(char)*MSG_LENGTH);
    
    
    
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
    
    int addrsize=sizeof(adr_distant);
    fd_set masterFD;//iniitalize FD
    FD_ZERO(&masterFD);
    FD_SET(sockListen, &masterFD);
    
    while(1){//accept connections and deal with read/write continuously
        fd_set readFD = masterFD;//reset readFD to full FD
        
        int socketCount = select(FD_SETSIZE,&readFD,NULL,NULL,NULL);
        if(socketCount==-1){
            printf("Failed select\r\n");
        }
        
        for (int sockClient = 0; sockClient < FD_SETSIZE; ++sockClient){
            if(FD_ISSET(sockClient,&readFD)){//for each pending socket
                if(sockClient==sockListen){//new connection !
                    int sockNew=accept(sockListen,(struct sockaddr*)&adr_distant,(socklen_t*)&addrsize);
                    if(sockNew==-1){
                        printf("Failed accept\r\n");
                    }
                    FD_SET(sockNew, &masterFD);
                    
                }else{//already connected client
                    
                    int l=read(sockClient,msg,MSG_LENGTH);
                    
                    if(l==-1){//error
                        printf("Failed Read\r\n");
                        exit(1);
                        
                    }else if(l==0){//close client
                        if(shutdown(sockClient,2)==-1){
                            printf("Failed shutdown sockClient\r\n");
                            exit(1);
                        }
                        if(close(sockClient)==-1){
                            printf("failed destructing sockClient\r\n");
                            exit(1);
                        }
                        FD_CLR(sockClient, &masterFD);
                        printf("Client disconnected\r\n");
                        
                    }else if(l>5){//read client
                        int id;
                        char *content=malloc((l-5)*sizeof(char));
                        sscanf(msg,"%d%s",&id,content);
                        printf("Reception (%d) [%s] a destination de %d\r\n",l,msg,id);
                        storeMsg(headUser,id,content);
                        
                    }else{//short
                        printf("Received shorter message than expected !\r\n");
                    }
                }
            }
        }
    }
    
    if(close(sockListen)==-1){
        printf("failed destructing socket\r\n");
        exit(1);
    }
    FD_CLR(sockListen, &masterFD);
    
    printf("Hello world !\r\n");
    return 0;
}
