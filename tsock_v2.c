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
	int tp=0;//0 for TCp, 1 for UDP
	while ((c = getopt(argc, argv, "pun:s")) != -1) {
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
		case 'u':
				tp=1;
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
	if(tp){
		sock=socket(AF_INET,SOCK_DGRAM,0);
		printf("Created UDP socket");
	}else{
		sock=socket(AF_INET,SOCK_STREAM,0);
		printf("Created TCP socket");
	}
	if(sock==-1){
		printf("Failed creating socket");
		exit(1);
	}

	if (source == 1){//send
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
		if(tp){//UDP
		//envoi
		for(int i=0;i<nb_message;i++){
			build_msg(msg,i,30);
			printf("SOURCE : Envoi n°%d,(30)[%s]\r\n",i+1,msg);
			if(sendto(sock,msg,30,0,(struct sockaddr*)&adr_distant,sizeof(adr_distant))==-1){
				printf("Error sendto");
			}
		}
		}else{//TCP
			//connect
			if(connect(sock,(struct sockaddr*)&adr_distant,sizeof(adr_distant))==-1){
				perror("connect");
				exit(1);
			}
			printf("Success connect\r\n");
			//envoi
			for(int i=0;i<nb_message;i++){
				build_msg(msg,i,30);
				int l=0;
				l=write(sock,msg,30);
				if(l==-1){
					printf("Error write");
				}
				printf("SOURCE : Envoi n°%d,(%d)[%s]\r\n",i+1,l,msg);
			}
			//shutdown
			if(shutdown(sock,2)==-1){
				printf("Failed shutdown socket\r\n");
				exit(1);
			}
		}


	}else{//source
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
		if(tp){//UDP
			//receivefrom
			int ind_msg=0;

			while(ind_msg<nb_message || nb_message==-1){
				int addrsize=sizeof(adr_distant);
				int l=0;
				printf("Start receivefrom\r\n");
				if ((l=recvfrom(sock,msg,30,0,(struct sockaddr*)&adr_distant,(socklen_t*)&addrsize))==-1){
					printf("Failed receive\r\n");
					exit(1);
				}
				if(l!=0)printf("PUIT: Reception n°%d (%d) [%s]\r\n",++ind_msg,l,msg);
			}
		}else{//TCP
			//listen
			if(listen(sock,1)==-1){
				printf("Failed listen\r\n");
				exit(1);
			}
			//accept
			int addrsize=sizeof(adr_distant);
			int sock_bis=accept(sock,(struct sockaddr*)&adr_distant,(socklen_t*)&addrsize);
			if(sock_bis==-1){
				printf("Failed accept\r\n");
			}
			//read
			int ind_msg=0;
			while(ind_msg<nb_message || nb_message==-1){
				int l=read(sock_bis,msg,30);
				if(l==-1){
					printf("Failed Read\r\n");
					exit(1);
				}
				if(l!=0)printf("TCP PUIT: Reception n°%d (%d) [%s]\r\n",++ind_msg,l,msg);
			}
			//shutdown
			if(shutdown(sock_bis,2)==-1){
				printf("Failed shutdown sock_bis\r\n");
				exit(1);
			}
			//close
			if(close(sock_bis)==-1){
				printf("failed destructing sock_bis\r\n");
				exit(1);
			}
		}
		

	}



	//destruct socket (commun T/R)
	if(close(sock)==-1){
		printf("failed destructing socket\r\n");
		exit(1);
	}
}

