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

 pid_t pid;

 //socket
 //int my_server_socket;
 char* recvBuff;
 //struct sockaddr_in server_addr;
 //struct hostent* host;

	//if((sockfd = socket(AF_INET, SOCK_STREAM, 0))<0)
	//{
	  //printf("Error: Could not create socket\n");
	  //return 1;
	//}

	//if((host = gethostbyname("127.0.0.1")) == NULL)		//host: 127.0.0.1
	//{
	  //printf("[CLIENT] Host Info Search - FAILED\n");
	//}

	/*
 	 * initialize struct to get a socket to host
	 */
	//memset (&server_addr, 0, sizeof (server_addr));
	//server_addr.sin_family = AF_INET;
	//memcpy (&server_addr.sin_addr, host->h_addr, host->h_length);
	//server_addr.sin_port = htons (8080);

	/*
      	* get a socket for communications
      	*/
	//printf ("[CLIENT] : Getting STREAM Socket to talk to SERVER\n");
	//fflush(stdout);
        //if ((my_server_socket = socket (AF_INET, SOCK_STREAM, 0)) < 0) 
        //{
          //printf ("[CLIENT] : Getting Client Socket - FAILED\n");
          //return 3;
        //}

	/*
         * attempt a connection to server
         */
	//printf ("[CLIENT] : Connecting to SERVER\n");
	//fflush(stdout);
        //if (connect (my_server_socket, (struct sockaddr *)&server_addr,sizeof    (server_addr)) < 0) 
     	//{
       	  //printf ("[CLIENT] : Connect to Server - FAILED\n");
          //close (my_server_socket);
          //return 4;
        //}

 	//*create function get message_key "getQueueKey()"
 	msgKey = ftok ("../../", 'M');
	if (msgKey == -1) 
	{ 
	  printf ("[CLIENT] Cannot create key!\n");
	  fflush (stdout);
	  return 0;
	}
	printf("msgkey: %d\n", msgKey);
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
		  shmKey = ftok(".", 16535);
		  if (shmKey == -1) 
	          { 
	  		printf ("(CLIENT) Cannot allocate key\n");
	  		return 1;
	          }

		  if ((shmid = shmget (shmKey, sizeof (MasterList), 0)) == -1) 
		  {
			printf ("(CLIENT) Shared-Memory doesn't exist. run the PRODUCER!\n");
			return 2;
	     	  }

		  msList = (MasterList *)shmat (shmid, NULL, 0);
	  	  if (msList == NULL) 
		  {
	  		printf ("(CLIENT) Cannot attach to Shared-Memory!\n");
	  		return 3;
		  }

		  int localnumDCs = msList->numberOfDCs;
		  if(localnumDCs == 10)
		  {
		    printf("There are already maximum DCs present(max 10)\n");
		    return 1;
		  }
		printf ("(CLIENT) Message queue ID: %d\n\n\n", mid);
		  break;
		}
	}
  	//*

 	//main process loop
	 while(1)
	 {
	   counter++;
	   machinePID = getpid();  	//dcmsg.machinePID
	   if(counter == 1)
	   {   
	     msg = getStatus(0);	//dcmsg.msg
	     send_message(mid, machinePID, msg);
	   }
	   else
	   {
	     iStatus = rand() % 7;
	     msg = getStatus(iStatus);		//dcmsg.msg
	     if(send_message(mid, machinePID, msg) == 1)
	     {
		return 1;
	     }
	     if(iStatus == 6)
	     {
	       counter = 0;
	       //clean up - closing socket
	       //close(my_server_socket);
	       break;
	     }
	   }
	   randSleep = (rand() % (30 - 10 + 1)) + 10;
	   sleep(randSleep);
	 }

 
 return 0;
}
