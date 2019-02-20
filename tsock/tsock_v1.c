/* librairie standard ... */
#include <stdlib.h>
/* pour getopt */
#include <unistd.h>
/* déclaration des types de base */
#include <sys/types.h>
/* constantes relatives aux domaines, types et protocoles */
#include <sys/socket.h>
/* constantes et structures propres au domaine UNIX */
#include <sys/un.h>
/* constantes et structures propres au domaine INTERNET */
#include <netinet/in.h>
/* structures retournées par les fonctions de gestion de la base de
   données du réseau */
#include <netdb.h>
/* pour les entrées/sorties */
#include <stdio.h>
/* pour la gestion des erreurs */
#include <errno.h>

void build_msg(char *arr,int index,int length){
  sprintf(arr,"%05d",index+1);
  for(int i=5;i<length;i++)arr[i]=97+(index%26);
		
}

void main (int argc, char **argv){
  int c;
  extern char *optarg;
  extern int optind;
  int nb_message = -1; /* Nb de messages à envoyer ou à recevoir, par défaut : 10 en émission, infini en réception */
  int source = -1 ; /* 0=puits, 1=source */
  while ((c = getopt(argc, argv, "pn:s")) != -1) {
    switch (c) {
    case 'p':
      if (source == 1) {
	printf("usage: cmd [-p|-s][-n ##]\n");
	exit(1);
      }
      source = 0;
      break;

    case 's':
      if (source == 0) {
	printf("usage: cmd [-p|-s][-n ##]\n");
	exit(1) ;
      }
      source = 1;
      break;

    case 'n':
      nb_message = atoi(optarg);
      break;

    default:
      printf("usage: cmd [-p|-s][-n ##]\n");
      break;
    }
  }

  if (source == -1) {
    printf("usage: cmd [-p|-s][-n ##]\n");
    exit(1) ;
  }

  if (nb_message != -1) {
    if (source == 1)
      printf("nb de tampons à envoyer : %d\n", nb_message);
    else
      printf("nb de tampons à recevoir : %d\n", nb_message);
  } else {
    if (source == 1) {
      nb_message = 10 ;
      printf("nb de tampons à envoyer = 10 par défaut\n");
    } else {
      printf("nb de tampons à envoyer = infini\n");
	}
  }
  //cree socket local (commun T/R)
  int sock;
  struct hostent *hp;
  struct sockaddr_in adr_distant;
  struct sockaddr_in adr_local;
  char msg[30]; //var global use in T/R

	if((sock=socket(AF_INET,SOCK_DGRAM,0))==-1){
    	printf("Failed creating socket");
    	exit(1);
  	}

  if (source == 1){
    printf("on est dans le source\n");
    //construct° addr
    memset((char*)& adr_distant,0,sizeof(adr_distant));
    adr_distant.sin_family=AF_INET;
    adr_distant.sin_port=htons(atoi(argv[argc-1]));

    if((hp=gethostbyname(argv[argc-2]))==NULL){
      printf("Failed gethostbyname\r\n");
      exit(1);
    }
    memcpy((char*)&(adr_distant.sin_addr.s_addr),hp->h_addr,hp->h_length);
	
    //envoi
    for(int i=0;i<nb_message;i++){
      build_msg(msg,i,30);
      printf("%s\r\n",msg);
      if(sendto(sock,msg,30,0,(struct sockaddr*)&adr_distant,sizeof(adr_distant))==-1){
	printf("Error sendto");
      }
    }


  }else{
    printf("on est dans le puits\n");

    //construct° addr
    memset((char*)& adr_local,0,sizeof(adr_local));
    adr_local.sin_family=AF_INET;
    adr_local.sin_port=htons(atoi(argv[argc-1]));
	adr_local.sin_addr.s_addr=INADDR_ANY;

	//bind 
	if (bind(sock,(struct sockaddr*)&adr_local,sizeof(adr_local))==-1){
		printf("fail bind\r\n");
		exit(1);
	}
	//receivefrom
    int ind_msg=0;
	
	while(ind_msg<nb_message || nb_message==-1){
		int addrsize=sizeof(adr_distant);
		int l=0;
		printf("Start receivefrom\r\n");
		if ((l=recvfrom(sock,msg,10,0,(struct sockaddr*)&adr_distant,(socklen_t*)&addrsize))==-1){
			printf("Failed receive\r\n");
			exit(1);
		}
		if(l!=0)printf("PUITS: Reception n°%d (%d) [%s]\r\n",++ind_msg,l,msg);
	}
	
  }

  
		
  //destruct socket (commun T/R)
  if(close(sock)==-1){
    printf("failed destructing socket");
    exit(1);
  }
}

