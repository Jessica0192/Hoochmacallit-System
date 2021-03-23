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
#include <sys/stat.h>

#include <signal.h>
#include "../../Common/inc/common.h"

#define TYPE_SERVERMESSAGE 1


int executeAction(int status, MasterList* masterls, int shmid);
int createLogMsgWOD(pid_t pid, int actionNum, int dcNum, char* msg);
