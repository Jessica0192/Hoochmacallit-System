/*
* FILE: dx.c
* PROJECT: A2
* PROGRAMMER: JESSICA SIM
* FIRST VERSION: 2021-02-11
* DESCRIPTION: This program takes several different options, input and output file name as optional.
* Depend on the existence of option, -srec, it creates SRecord or ASM output. If user choose option '-h'
* it will output help(usage statement) information and exit the program. If there is no input and output file name
* provided in argument, it gets standard input and prints as standard output
*/

#include "../inc/dx.h"


int main(int argc, char* argv[])
{
 DCInfo dcinfo;
 DCMessage dcmsg;
 MasterList *p;

 key_t shmKey;
 key_t message_key;

 int shmid; // shared memory ID
 int msgmid; //message queue ID

 int counter=0;
 char* strStatus = NULL;
 int iStatus=0;
 int randSleep=0;
 int wodStatus=0;
 int retVal = 0;
 struct tm tm; 

 time_t T = time(NULL);

 	//message queue
 	shmKey = ftok (".", 'M');
	if (shmKey == -1) 
	{ 
	  printf ("(CLIENT) Cannot create key!\n");
	  fflush (stdout);
	  return 0;
	}

	// check if share memory exists
	while(1)
	{
		shmid = shmget(shmKey, sizeof(MasterList), 0);
		if (shmid == -1) 
		{
		  counter++;
		  printf("(DX)Waiting\n");
		  sleep(10);
		}
		else
		{
		  p = (MasterList *)shmat (shmid, NULL, 0);
		  if (p == NULL) 
		  {
		    printf ("(DX) Cannot attach to shared memory!\n");
		    return 3;
		  }
		  break;
		}

		if(counter == 100)
		{
		  return 1;
		}
	}
  	//*

 	//main process loop
	 while(1)
	 {
	   randSleep = (rand() % (30 - 10 + 1)) + 10;
	   sleep(randSleep);
	   message_key = ftok (".", 'M');
	   if (message_key == -1) 
	   { 
	     printf ("(CLIENT) Cannot create key!\n");
	     fflush (stdout);
	     return 0;
	   }


	   msgmid = msgget (message_key, 0);
	   if (msgmid == -1) 
	   {
		tm = *localtime(&T);
		createLogMsgWOD(tm, 0, 0, 0, "DX detected that msgQ is gone - assuming DR/DCs done");
		  break;
	   }
	   iStatus = rand() % 21;
	   retVal = executeAction(iStatus, p);
	   if(retVal == 1)
	   {
		return 1;
	   }
	 }

 return 0;
}
