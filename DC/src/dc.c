/*
* FILE: encodeInput.c
* PROJECT: A2
* PROGRAMMER: JESSICA SIM
* FIRST VERSION: 2021-02-11
* DESCRIPTION: This program takes several different options, input and output file name as optional.
* Depend on the existence of option, -srec, it creates SRecord or ASM output. If user choose option '-h'
* it will output help(usage statement) information and exit the program. If there is no input and output file name
* provided in argument, it gets standard input and prints as standard output
*/

#include "../inc/dc.h"


int main(int argc, char* argv[])
{
 DCInfo dcinfo;
 DCMessage dcmsg;
 key_t msgKey;	//message key
 int mid; // message ID
 int counter=0;
 char* strStatus = NULL;
 int iStatus=0;
 int randSleep=0;

 //*create function get message_key "getQueueKey()"
 	msgKey = ftok (".", 'M');
	if (msgKey == -1) 
	{ 
	  printf ("(CLIENT) Cannot create key!\n");
	  fflush (stdout);
	  return 0;
	}

	// check if the msg queue already exists
	while(1)
	{
		mid = msgget (msgKey, 0);
		if (mid == -1) 
		{
		  printf("(DC)Waiting\n");
		  sleep(10);
		}
		else
		{
		  break;
		}
	}
  	//*

 	//main process loop
	 while(1)
	 {
	   counter++;
	   dcmsg.machinePID = getpid();  
	   if(counter == 1)
	   {   
	     dcmsg.msg = getStatus(0);
	     send_message(mid);
	   }
	   else
	   {
	     iStatus = rand() % 7;
	     dcmsg.msg = getStatus(iStatus);
	     send_message(mid);
	     if(iStatus == 6)
	     {
	       counter = 0;
	       break;
	     }
	   }
	   randSleep = (rand() % (30 - 10 + 1)) + 10;
	   sleep(randSleep);
	 }

 return 0;
}
