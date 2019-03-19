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
#define MSG_MAX_LENGTH 30
#define SERVERMODE 0
#define CLIENTPOSTMODE 1
#define CLIENTREADMODE 2

BAL_user *headUser;

void build_msg(char *arr,int id,int index,int length){
    sprintf(arr,"%05d",id);
    for(int i=5;i<length;i++)arr[i]=97+(index%26);
    
}

int main(int argc, char **argv){
    int c;
    extern char *optarg;
    extern int optind;
    
    int mode=SERVERMODE;
    int BALid=0;
    int nb_message = 1;
    
    while ((c = getopt(argc, argv, "e:br:n:")) != -1) {
        switch (c) {
            case 'b':
                mode=SERVERMODE;
                break;
                
            case 'r':
                BALid= atoi(optarg);
                mode=CLIENTREADMODE;
                break;
                
            case 'n':
                nb_message = atoi(optarg);
                break;
            case 'e':
                BALid= atoi(optarg);
                mode=CLIENTPOSTMODE;
                break;
            default:
                printf("WHATDIDYOUDO!usage: cmd [-p|-s|-b|-r#|-e #][-n ##] [hostName] portName\n");
                break;
        }
    }
    
    
    
    
    
    headUser=malloc(sizeof(BAL_user));
    
    int sockListen;
    struct hostent *hp;
    struct sockaddr_in adr_distant;
    struct sockaddr_in adr_local;
    
    char *msg;
    msg=malloc(sizeof(char)*MSG_MAX_LENGTH);
    
    
    
    sockListen=socket(AF_INET,SOCK_STREAM,0);
    if(sockListen==-1){
        printf("Failed creating sockListen");
        exit(1);
    }
    
    if(mode==SERVERMODE){ //serveur part
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
        
        printf("PUITS : port=%s, TP=tcp, \r\n",argv[argc-1]);
        
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
                        printf("New connection !\r\n");
                    }else{//already connected client
                        
                        int l=read(sockClient,msg,MSG_MAX_LENGTH);
                        
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
                            //if recup
                            if(content[0]=='.'){
                                //read user
                                printf("Demande de recup user n%d\r\n",id);
                                readUser(headUser, id, sockClient);
                                
                                if(shutdown(sockClient,2)==-1){
                                    printf("Failed shutdown sockClientRecup\r\n");
                                    exit(1);
                                }
                                if(close(sockClient)==-1){
                                    printf("failed destructing sockClientRecup\r\n");
                                    exit(1);
                                }
                                FD_CLR(sockClient, &masterFD);
                                printf("Client recup disconnected\r\n");
                            }else{
                                //store
                                printf("PUITS : Réception et stockage lettre pour le recepteur n°%d[%s]\r\n",id,msg); //missing num letter
                                storeMsg(headUser,id,content,l-5);
                            }
                            
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
        
    }else{//client part
        memset((char*)& adr_distant,0,sizeof(adr_distant));
        adr_distant.sin_family=AF_INET;
        adr_distant.sin_port=htons(atoi(argv[argc-1]));
        
        if((hp=gethostbyname(argv[argc-2]))==NULL){
            printf("Failed gethostbyname\r\n");
            exit(1);
        }
        memcpy((char*)&(adr_distant.sin_addr.s_addr),hp->h_addr,hp->h_length);
        if(connect(sockListen,(struct sockaddr*)&adr_distant,sizeof(adr_distant))==-1){
            printf("connect error!\r\n");
            exit(1);
        }
        if (mode==CLIENTPOSTMODE){ //clientpost part
            for(int i=0;i<nb_message;i++){
                printf("SOURCE : lg_mesg_emis=%d, port=%s, nb_d'envois=%d, TP=tcp, dest=%s\r\n",MSG_MAX_LENGTH,argv[argc-1],nb_message,argv[argc-2]); //missing long msg,
                build_msg(msg,BALid,i,MSG_MAX_LENGTH);
                int l=0;
                l=write(sockListen,msg,MSG_MAX_LENGTH);
                if(l==-1){
                    printf("Error write\r\n");
                }
                printf("SOURCE : Envoi lettre n°%d à destination du récepteur %d (%d)[%s]\r\n",i+1,BALid,MSG_MAX_LENGTH,msg);
            }
        }else if(mode==CLIENTREADMODE){//clientread part
            sprintf(msg,"%05d...............",BALid);
            int l=0;
            l=write(sockListen,msg,MSG_MAX_LENGTH);
            if(l==-1){
                printf("Error write\r\n");
            }
            printf("Reception : Demande recuperation de ses lettres par le récepteur n°%d port=%s, TP=tcp, dest=%s\r\n",BALid,argv[argc-1],argv[argc-2]);
            int ind_msg=0;
            while(l!=0){
                l=read(sockListen,msg,MSG_MAX_LENGTH);
                if(l==-1){
                    printf("Failed Read\r\n");
                    exit(1);
                }
                ind_msg++;
                if(l>0)printf("RECEPTION: Récupération lettre n°%d par le récepteur  n°%d (%d) [%s]\r\n",ind_msg,BALid, MSG_MAX_LENGTH, msg);
            }
            if(ind_msg==0){
                printf("boite déjà vide\r\n");
            }
        }else{//client mode error
            printf("what the heck men???\r\n");
        }
        //shutdown
        if(shutdown(sockListen,2)==-1){
            printf("Failed shutdown socket\r\n");
            exit(1);
        }
    }
    
    return 0;
}
