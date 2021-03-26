/*
* FILE          : common.h
* PROJECT       : PROG2121 - Assignment #3
* PROGRAMMER    : Jessica Sim, Maria Malinina
* FIRST VERSION : 2020-03-14
* DESCRIPTION   :
* This file contains all the #includes, constants and function prototypes
* that the data reader program uses.
*/
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/shm.h>

#define MAX_DC_ROLES 10
#pragma warning (disable: 4996)

typedef struct  
{
	// the following is a requriement of UNIX/Linux
	long type;

	// now the specifics of our message
	pid_t machinePID;
	char msg[100];
	
} DCMessage;


typedef struct  
{
	int hours;
	int minutes;
	int seconds;
	int time;
	
} last_time;

typedef struct  
{
	// the following is a requriement of UNIX/Linux
	long type;

	// now the specifics of our message
	pid_t dcProcessID;
	last_time lastTimeHeardFrom;	//to calculate difference use "double difftime ( time_t time2, time_t time1 ); " 
	
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

