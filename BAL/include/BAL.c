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


void storeMsg(BAL_user *headUser, int id, char *msg){
	//search for user id
	BAL_user *cur=headUser;
	if(cur->nextUser!=NULL){
		cur=headUser->nextUser;
		int found=0;
		while(!found && (cur->nextUser != NULL)){
			if(cur->id == id){
				found=1;
			}else{
				cur=cur->nextUser;
			}
		}
		//if user exists
		if(found){
			addMsg(cur, msg);
			printf("storing msg=%s in existing user n%d\r\n",msg,cur->id); 
		}
	}
	//else create user
	cur->nextUser=malloc(sizeof(BAL_user));
	cur->nextUser->id=id;
	//add msg
	addMsg(cur->nextUser, msg);
	printf("storing msg=%s in new user n%d\r\n",cur->nextUser->firstMsg->msg,cur->nextUser->id);

}
