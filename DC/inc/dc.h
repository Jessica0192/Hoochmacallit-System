#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>

#define MAX_DC_ROLES 10
#define TYPE_SERVERMESSAGE 1

#define EVERYTHING_OK 0
#define HYDRAULIC_FAIL 1
#define SAFETY_BTN_FAIL 2
#define NO_RAW_MATERIAL 3
#define OPERATING_TEMP_OUT_OF_RANGE 4
#define OPERATOR_ERR 5
#define MACHINE_OFF_LINE 6

#include "../../Common/inc/common.h"


char* getStatus(int status);
int send_message (int mid, pid_t pid, char* msg);
int createLog(int status, pid_t pid, char* msg);
