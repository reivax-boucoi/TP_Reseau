#include "BAL.h"


void readMsg(BAL_msg *msg){
	printf("%s\r\n",msg->msg);
	if(msg->nextMsg != NULL){
		readMsg(msg->nextMsg);
		free(msg->nextMsg);
		msg->nextMsg=NULL;
	}
}

void readUser(BAL_user *user){
	if(user->firstMsg != NULL){
		printf("clean boite n%d\r\n", user->id);
		readMsg(user->firstMsg);
		free(user->firstMsg);
		user->firstMsg=NULL;
	}else{
		printf("boite n%d deja vide\r\n", user->id);
	}
}

void addMsg(BAL_user *user, char *msg){
	if(user->firstMsg != NULL){
		BAL_msg *cur = user->firstMsg;
		while(cur->nextMsg != NULL){
			cur = cur->nextMsg;
		}
		cur->nextMsg = malloc(sizeof(BAL_msg));
		cur->nextMsg->msg=msg;
	}else{
		user->firstMsg = malloc(sizeof(BAL_msg));
		user->firstMsg->msg = msg;
	}
}	
		
