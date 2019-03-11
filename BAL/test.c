#include <stdio.h>
#include <stdlib.h>
#include "include/BAL.h"

BAL_user *headUser;
BAL_user *firstUser;

int main(void){
	headUser=malloc(sizeof(BAL_user));
	firstUser=malloc(sizeof(BAL_user));
	headUser->nextUser=firstUser;
	char msg[]="Hello here!";
	char msg1[]="Hello there!";
	firstUser->id=3;
	addMsg(firstUser,msg);
	addMsg(firstUser,msg1);
	printf("msg = %s\r\n",firstUser->firstMsg->msg);
	printf("msg1 = %s\r\n",firstUser->firstMsg->nextMsg->msg);
	printf("Hello world !\r\n");
	readUser(firstUser);
	readUser(firstUser);
	return 0;
}
