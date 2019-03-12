#ifndef _BAL_H_
#define _BAL_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netdb.h>

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

BAL_user *searchUser(BAL_user *headUser,int id);
void readUser(BAL_user *headUser,int id,int sock);
void readMsg(BAL_msg *msg,int sock);
void addMsg(BAL_user *user, char *msg);
void storeMsg(BAL_user *headUser,int id, char *msg);

#endif
