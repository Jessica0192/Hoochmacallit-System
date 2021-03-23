/*
* FILE: dx.c
* PROJECT: A3
* PROGRAMMER: JESSICA SIM
* FIRST VERSION: 2021-03-11
* DESCRIPTION: This program is a corruptor application and the purpose of this application is to create the alternative
* (or exceptions) paths through the DC and DR applications. It does two actions: killing a DC process to test DC
* application going offline in the application suite, deleting the message queue being used between set of DC
* applications and DR application 
*/

#include "../inc/dx.h"

int main(int argc, char* argv[])
{
 //DCInfo dcinfo;
 //DCMessage dcmsg;
 MasterList* p;

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
 int shmSize = 0;

	//p = malloc(10 * sizeof(MasterList));

 	//get key of shared-memory ID
 	shmKey = ftok (".", 16535);
	if (shmKey == -1) 
	{ 
	  //printf ("(CLIENT) Cannot create key!\n");
	  //fflush (stdout);
	  return 0;
	}
	shmSize = sizeof(MasterList) - sizeof(long);
	// check if shared memory exists
	while(1)
	{
		shmid = shmget(shmKey, shmSize, 0);
		//printf("shmid: %d\n", shmid);
		if (shmid == -1) 
		{
		  counter++;
		  //printf("(DX)Waiting\n");
		  sleep(10);
		}
		else
		{
		  p = (MasterList *)shmat (shmid, NULL, 0);
		  if (p == NULL) 
		  {
		    //printf ("(DX) Cannot attach to shared memory!\n");
		    return 3;
		  }
		  break;
		}

		//if the application waited for 10 times and the shared memory still is not existing, terminates
		//the application
		if(counter == 100)
		{
		  return 1;
		}
	}

	//get message key
	message_key = ftok ("../../", 'M');
	if (message_key == -1) 
	{ 
          //printf ("(CLIENT) Cannot create key!\n");
	  //fflush (stdout);
	  return 0;
	}

 	//main process loop
	 while(1)
	 {
           srand ( time(NULL) );
	   //get random amount of time
	   randSleep = (rand() % (30 - 10 + 1)) + 10;
	   //go to sleep for random amount of time(between 10 and 30 secs)
	   sleep(randSleep);
	   
	   //check for existence of message queue(between DC's and DR)
	   msgmid = msgget (message_key, 0);
	   if (msgmid == -1) 
	   {
		//printf("msgmid == -1\n");
		createLogMsgWOD(0, 0, 0, "DX detected that msgQ is gone - assuming DR/DCs done");
		if(-1 == shmdt(p))
		{
			return -1;
		}
		  break;	//or return 1;
	   }

	   //get random number of action(Wheel of Destruction)
           srand ( time(NULL) );
	   iStatus = rand() % 21;
	   retVal = executeAction(iStatus, p);
	   if(retVal == 1 || retVal == 2)
	   {
		return 1;
	   }
	 }

 return 0;
}
