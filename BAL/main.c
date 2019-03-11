#include <stdio.h>
#include <stdlib.h>
#include "include/BAL.h"

BAL_user *headUser;
BAL_user *firstUser;

int main(void){
	headUser=malloc(sizeof(BAL_user));
	firstUser=malloc(sizeof(BAL_user));
	headUser->nextUser=firstUser;
	
	printf("Hello world !\r\n");
	return 0;
}
