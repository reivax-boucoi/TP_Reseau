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

//build message to send
	void build_msg(char *arr,int index,int length){
	  sprintf(arr,"%05d",index+1);
	  for(int i=5;i<length;i++)arr[i]=97+(index%26);
	}

//cree socket local (commun T/R)
	int create_sock(int tp){
		if(tp){
			sock=socket(AF_INET,SOCK_DGRAM,0);
		}else{
			sock=socket(AF_INET,SOCK_STREAM,0);
		}
		if(sock==-1){
			printf("Failed creating socket");
			exit(1);
		}
	}

int main (int argc, char **argv){
	int c;
	extern char *optarg;
	extern int optind;
	int nb_message = -1; /* Nb de messages à envoyer ou à recevoir, par défaut : 10 en émission, infini en réception */
	int source = -1 ; /* 0=puits, 1=source */
	int tp=0; /* 0 for TCp, 1 for UDP */
	int msg_length=30;
	
	while ((c = getopt(argc, argv, "pun:l:s")) != -1) {
		switch (c) {
		case 'p':
			if (source == 1) {
			printf("1usage: cmd [-p|-s][-n ##]\n");
			exit(1);
			}
			source = 0;
			break;

		case 's':
		if (source == 0) {
			printf("2usage: cmd [-p|-s][-n ##]\n");
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
		case 'l':
			msg_length = atoi(optarg);
			break;
		default:
			printf("3usage: cmd [-p|-s][-n ##]\n");
			break;
		}
	}

	/*if (source == -1) {
		printf("3usage: cmd [-p|-s][-n ##]\n");
		exit(1) ;
	}*/

	if ((nb_message != -1)&&(source == 1)) nb_message = 10 ;

	/*if (nb_message != -1) {
		if (source == 1){
			//printf("nb de tampons à envoyer : %d\n", nb_message);
		}else{
			//printf("nb de tampons à recevoir : %d\n", nb_message);
		}
	} else {
		if (source == 1) {
			nb_message = 10 ;
			//printf("nb de tampons à envoyer = 10 par défaut\n");
		} else {
			//printf("nb de tampons à envoyer = infini\n");
		}
	}*/
	
	char *msg; //var global use in T/R
	msg=malloc(sizeof(char)*msg_length);
	int sock;
	struct hostent *hp;
	struct sockaddr_in adr_distant;
	struct sockaddr_in adr_local;	
	
	
	
	create_sock(tp);//cree socket local (commun T/R)
	
	if (source == 1){//send
		printf("SOURCE : lg_mesg_emis=%d, port=%s, nb_envois=%d, TP=%s, dest=%s\r\n",msg_length,argv[argc-1],nb_message,tp?"udp":"tcp",argv[argc-2]);
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
			build_msg(msg,i,msg_length);
			printf("SOURCE : Envoi n°%d,(30)[%s]\r\n",i+1,msg);
			if(sendto(sock,msg,msg_length,0,(struct sockaddr*)&adr_distant,sizeof(adr_distant))==-1){
				printf("Error sendto");
			}
		}
		}else{//TCP
			//connect
			if(connect(sock,(struct sockaddr*)&adr_distant,sizeof(adr_distant))==-1){
				perror("connect");
				exit(1);
			}
			//envoi
			for(int i=0;i<nb_message;i++){
				build_msg(msg,i,msg_length);
				int l=0;
				l=write(sock,msg,msg_length);
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
	printf("SOURCE : fin\r\n");
	}else{//pouit
		printf("PUITS : lg_mesg_lu=%d, port=%s, nb_receptions=",msg_length,argv[argc-1]);
		if(nb_message==-1){
			printf("infini");
		}else{
			printf("%d",nb_message);
		}
		printf(", TP=%s\r\n",tp?"udp":"tcp");
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
				if ((l=recvfrom(sock,msg,msg_length,0,(struct sockaddr*)&adr_distant,(socklen_t*)&addrsize))==-1){
					printf("Failed receive\r\n");
					exit(1);
				}
				if(l!=0)printf("PUITS: Reception n°%d (%d) [%s]\r\n",++ind_msg,l,msg);
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
				int l=read(sock_bis,msg,msg_length);
				if(l==-1){
					printf("Failed Read\r\n");
					exit(1);
				}
				if(l!=0)printf("PUITS: Reception n°%d (%d) [%s]\r\n",++ind_msg,l,msg);
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
		printf("PUITS : fin\r\n");
	}



	//destruct socket (commun T/R)
	if(close(sock)==-1){
		printf("failed destructing socket\r\n");
		exit(1);
	}
	return 1;
}
