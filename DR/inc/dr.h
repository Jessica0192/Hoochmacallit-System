
/*
* FILE          : dr.h
* PROJECT       : PROG2121 - Assignment #3
* PROGRAMMER    : Maria Malinina
* FIRST VERSION : 2020-03-14
* DESCRIPTION   :
* This file contains all the #includes, constants and function prototypes
* that the data reader program uses.
*/


#include <stdbool.h>
#define FIRST_SLEEP 15
#define LAST_SLEEP 1.5


#include "../../Common/inc/common.h"

void removeDC(char* strCount, char*strProcessID, char* rem_dc_log, FILE* log_stream, int status);
void rearrange(int removalID, int localNumDCs, MasterList* msList, int DC_pids[]);
void removeLogic(int localNumDCs, MasterList* msList, int removalID, char* strCount, char* strProcessID, int DC_pids[], char* rem_dc_log, FILE* log_stream);
