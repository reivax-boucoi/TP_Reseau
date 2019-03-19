#include "BAL.h"


void readMsg(BAL_msg *msg,int sock){
	printf("%s\r\n",msg->msg);
	if(write(sock,msg->msg,25*sizeof(char))==-1){
        printf("failed responding to client\r\n");
        exit(1);
    }
	if(msg->nextMsg != NULL){
		readMsg(msg->nextMsg,sock);
		free(msg->nextMsg);
		msg->nextMsg=NULL;
	}
}

void readUser(BAL_user *headUser,int id, int sock){
	BAL_user *user=searchUser(headUser,id);
	if(user->firstMsg != NULL){
		printf("clean boite n%d\r\n", user->id);
		readMsg(user->firstMsg,sock);
		free(user->firstMsg);
		user->firstMsg=NULL;
	}else{
		printf("boite n%d deja vide\r\n",id);
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

BAL_user *searchUser(BAL_user *headUser, int id){
	BAL_user *cur=headUser;
	int found=0;
	while(!found && (cur->nextUser != NULL)){
		cur=cur->nextUser;
		if(cur->id == id){
			found=1;
		}
	}
	return cur;
}	 


void storeMsg(BAL_user *headUser,int id, char *msg){
	BAL_user *cur=searchUser(headUser,id);
	//if user exists
	if(cur->id==id){
		addMsg(cur, msg);
		printf("storing msg=%s in existing user n%d\r\n",msg,cur->id); 
	}else{
		//else create user
		cur->nextUser=malloc(sizeof(BAL_user));
		cur->nextUser->id=id;
		//add msg
		addMsg(cur->nextUser, msg);
		printf("storing msg=%s in new user n%d\r\n",cur->nextUser->firstMsg->msg,cur->nextUser->id);
	}
}
