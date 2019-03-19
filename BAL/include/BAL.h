#ifndef _BAL_H_
#define _BAL_H_

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

typedef struct BAL_msg BAL_msg;
struct BAL_msg{
	char * msg;
	struct BAL_msg *nextMsg;
};

typedef struct BAL_user BAL_user;
struct BAL_user{
	int id;
	struct BAL_msg *firstMsg;
	struct BAL_user *nextUser;
};

BAL_user *searchUser(BAL_user *headUser,int id);	//search for user by id, returns headuser if not found
void readUser(BAL_user *headUser,int id,int sock);  //empty user's messages into socket'
void readMsg(BAL_msg *msg,int sock); // recursive read of all the messages from the first downwards
void addMsg(BAL_user *user, char *msg); //adds a message to a given user
void storeMsg(BAL_user *headUser,int id, char *msg);    //finds a user (or creates him) and stores his message

#endif
