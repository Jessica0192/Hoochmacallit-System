/*
* FILE          : dc.h
* PROJECT       : PROG2121 - Assignment #3
* PROGRAMMER    : Jessica Sim
* FIRST VERSION : 2020-03-14
* DESCRIPTION   :
* This file contains all the #includes, constants and function prototypes
* that the data reader program uses.
*/

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
