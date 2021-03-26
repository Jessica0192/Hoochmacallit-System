/*
* FILE          : dx.h
* PROJECT       : PROG2121 - Assignment #3
* PROGRAMMER    : Jessica Sim
* FIRST VERSION : 2020-03-14
* DESCRIPTION   :
* This file contains all the #includes, constants and function prototypes
* that the data reader program uses.
*/

#include <sys/types.h>
#include <signal.h>

#include "../../Common/inc/common.h"

#define TYPE_SERVERMESSAGE 1


int executeAction(int status, MasterList* masterls);
int createLogMsgWOD(pid_t pid, int actionNum, int dcNum, char* msg);
