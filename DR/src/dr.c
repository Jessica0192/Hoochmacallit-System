/*
* FILE: dc.c
* PROJECT: SENG2030 - Assignment #3
* PROGRAMMER: MARIA MALININA
* FIRST VERSION: 2021-03-11
* DESCRIPTION: This program creates the message key with the specific key('M') and check if the message queue is 
* existing with the key just created. If the message queue is not existing, wait until it is created. 
* If the message queue is existing, it goes to main loop which gets a random number of status and send
* the corresponding message of that status to server. If the status is 6, it sends the coressponding message and 
* terminates the current DC application
*/

#include "../inc/dc.h"


int main(int argc, char* argv[])
{
 //DCMessage dcmsg;
 MasterList* msList;

 key_t msgKey;	//message key
 key_t shmKey;  //share memory key
 int mid; // message ID
 int shmid; //share memory ID

 int counter=0;
 char* strStatus = NULL;
 int iStatus=0;
 int randSleep=0;
 pid_t machinePID;
 char* msg;
 int numOfClients = 0;

 struct tm tm; 
 time_t T = time(NULL);
 pid_t pid;

 char* recvBuff;

	//get message key
 	msgKey = ftok ("../../", 'M');
	if (msgKey == -1) 
	{ 
	  return 0;
	}

	// check if the msg queue already exists
	while(1)
	{
		mid = msgget (msgKey, 0);

		if (mid == -1) 
		{
		  //printf("(DC)Waiting\n");
		  sleep(10);
		}
		else
		{
		  //printf ("(CLIENT) Message queue ID: %d\n\n\n", mid);
		  break;
		}
	}

 	//main process loop
	 while(1)
	 {
	   counter++;
	   iStatus = 0;
 	   srand ( time(NULL) );
	   //get the pid of the current DC application
	   machinePID = getpid();  				//dcmsg.machinePID
	   //to check if the first time that current DC is connected
	   if(counter == 1)
	   {   
	     //set iStatus to 0 because it is a first time connecting
	     iStatus = 0;
	     //get the message corresponding of the status
	     msg = getStatus(iStatus);				//dcmsg.msg
	     //send message
	     if(send_message(mid, machinePID, msg) == 1)
	     {
		printf("here\n");
		return 1;
	     }
	   }
	   else
	   {
	     //get random number of status between 0 to 6
	     iStatus = rand() % 7;
	     //get the message corresponding of the status
	     msg = getStatus(iStatus);				//dcmsg.msg
	     //send message
	     if(send_message(mid, machinePID, msg) == 1)
	     {
		return 1;
	     }
	   }
	   //create log message after sending message
	   createLog(iStatus, machinePID, msg);
	   //if the status was 6, terminates the current application
	   if(iStatus == 6)
	   {
	     //kill(pid, SIGKILL);
             break;
           }
	   //get random number from 10 to 30
	   randSleep = (rand() % (30 - 10 + 1)) + 10;
	   //go to sleep for amount of randSleep 
	   sleep(randSleep);
	 }

 
 return 0;
}
