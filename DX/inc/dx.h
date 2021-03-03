#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/types.h>

#include <signal.h>

#define MAX_DC_ROLES 10
#define TYPE_SERVERMESSAGE 1


typedef struct  
{
	// the following is a requriement of UNIX/Linux
	long type;

	// now the specifics of our message
	pid_t dcProcessID;
	time_t lastTimeHeardFrom;	//to calculate difference use "double difftime ( time_t time2, time_t time1 ); " 
	
} DCInfo;


typedef struct  
{
	// the following is a requriement of UNIX/Linux
	long type;

	// now the specifics of our message
	int msgQueueID;
	int numberOfDCs;
	DCInfo dc[MAX_DC_ROLES];
	
} MasterList;

typedef struct  
{
	// the following is a requriement of UNIX/Linux
	long type;

	// now the specifics of our message
	pid_t machinePID;
	char* msg;
	
} DCMessage;

int executeAction(int status, MasterList* masterls);
int createLogMsgWOD(struct tm T, pid_t pid, int actionNum, int dcNum, char* msg);
