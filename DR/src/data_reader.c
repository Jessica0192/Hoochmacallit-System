#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#pragma warning (disable: 4996)
#define FIRST_SLEEP 15
#define LAST_SLEEP 1.5
#define TYPE_SERVERMESSAGE		1
#define MAX_DC_ROLES 	10 
#define OPERATION_ADD			1
#define OPERATION_DELETE		2
#define OPERATION_LIST			3
#define OPERATION_EXIT			4

#define OPERATION_SUCCESS		0
#define OPERATION_DBFAILURE		1
#define OPERATION_INVALIDDATA		2
#define SERVER_EXIT			10
#define DUMB_CLIENT_ERROR		999
#define KILL_SERVER	0
#define	GET_DATE	1
#define GET_TIME	2
#define GET_RND_NUM	3
#define ADD_VALUES	4
#define TERM_SERVER	5
#define TERM_CLIENT	6

#define CLIENT_2_MSG	1

#define _CRT_SECURE_NO_WARNINGS



typedef struct  
{
	// the following is a requriement of UNIX/Linux
	long type;

	// now the specifics of our message
	pid_t dcProcessID;
	char* lastTimeHeardFrom;	//to calculate difference use "double difftime ( time_t time2, time_t time1 ); " 
	
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

typedef struct  
{
	// the following is a requriement of UNIX/Linux
	long type;

	// now the specifics of our message
	pid_t machinePID;
	char msg[100];
	
} DCMessage;

//key_t ftok(char* path, int id);
int msgget(key_t key, int msgFlag);
//int shmget(key_t key, int memSize, int memFLag);
void delay(int milliseconds);

int main(void)
{
    int mid; // message queue ID
    int sizeofdata;
    int continueToRun = 1;
   

    key_t message_key;
    MasterList masterls;
  //  MasterList response;
    FILE* log_stream;

    if ((log_stream = fopen("server_log.txt", "w+")) == NULL) {
            fprintf(stderr, "%s: %s\n", "Impossible to create a file", "server_log.txt");
	    printf("Error opening file\n");
            return -1;
        }

    struct stat st; 
    int ret = stat("server_log.txt", &st);
     if(ret != 0) {
         return -1;
    }   

    int mode = W_OK;
    if(access("server_log.txt", mode) ==0) {
	printf("can write \n");
    } 
    else
    {
	printf("no permission\n");
	return -1;
    }


    // get the unique token for the message queue (based on some agreed 
    // upon "secret" information  
    message_key = ftok ("../../", 'M');
    if (message_key == -1) 
    { 
        printf ("(SERVER) Cannot create key!\n");
        fflush (stdout);
        return 1;
    }

printf("message_key:%d\n", message_key);

    // create the message queue
    mid = msgget (message_key, IPC_CREAT | 0660);
    if (mid == -1) 
    {
        printf ("(SERVER) ERROR: Cannot create queue\n");
        fflush (stdout);
        return 2;
    }

    printf ("(SERVER) Message queue ID: %d\n\n", mid);
    fflush (stdout);
    masterls.msgQueueID = mid;

    //msgctl (mid, IPC_RMID, NULL);
    //return 1;

    //now create (or find a) shared mem obj
    //the function will retunr -1 if the sys
    //is unable to create the obj
    int segment_id;
    char bogus;
    char* shared_memory;
    //struct shmid_ds shmbuffer;
    int segment_size;
    const int shared_segment_size = 0x6400;
    /* Allocate a shared memory segment.  */
    segment_id = shmget (IPC_PRIVATE, shared_segment_size, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
    /* Attach the shared memory segment.  */
    printf("Shared memory segment ID is %d\n", segment_id);
    shared_memory = (char*) shmat (segment_id, 0, 0);
    printf ("shared memory attached at address %p\n", shared_memory);
    /* Determine the segment's size. */
    printf("Message queue id is %d", mid);
    /*
    shmctl (segment_id, IPC_STAT, &shmbuffer);
    segment_size  =               shmbuffer.shm_segsz;
    printf ("segment size: %d\n", segment_size);
    */
    /* Write a string to the shared memory segment.  */
    sprintf (shared_memory, "Hello, world.");
    /* Detach the shared memory segment.  */
    shmdt (shared_memory);
    printf("Wrote Hello World to the segment\n");
	

	//sleep for 15 seconds
	//delay(15000);
    sleep(FIRST_SLEEP);



    // compute size of data portion of message //IT WAS MASTERLIST
    sizeofdata = sizeof (MasterList) - sizeof (long);
    clock_t start;
    clock_t end;
    double diff_t;
    float seconds = 0;
    int done = 0;
    int rc;	// return code from message processing


   // time(&start_t);
   int DC_count = 0;
   int localNumDCs = 0;
   char new_dc_log[255];
   char rem_dc_log[255];
   char strCount[5] = {0};		//string count of DCs when writing to file
   char strProcessID[20] = {0};		//string processID when writing to file
   memset(new_dc_log ,0, sizeof(new_dc_log));
   memset(rem_dc_log ,0, sizeof(rem_dc_log));
  // time_t strart = time(NULL);
  DCMessage incom_msg;

  time_t rawtime;
  struct tm* timeinfo;


  //MAIN LOOP
    while (1)
        {
	   memset(strCount, 0, sizeof(strCount));
	   memset(strProcessID,0,sizeof(strProcessID));

            printf ("(SERVER) Waiting for a message ...\n");
             
                // compute size of data portion of message //IT WAS MASTERLIST
                int new_size = sizeof (DCMessage) - sizeof (long);


           // rc = msgrcv (mid, (void *)&msg, sizeof (DCInfo), 0, 0); //DCMessage
            rc = msgrcv(mid, &incom_msg, new_size,0 ,0);
		printf("incom_msg pid: %d\n", incom_msg.machinePID);
		printf("incom_msg msg: %s\n", incom_msg.msg);
		printf("rc: %d\n", rc);

            if (rc == -1) 
            {
                printf("(SERVER) Error occured while trying to receive a message...");
               // break;
               end = clock();
               seconds += (float)(end - start) / CLOCKS_PER_SEC;

               if (seconds == 35)
               {
		localNumDCs--;
                masterls.numberOfDCs = localNumDCs;
                seconds = 0; //reset
		
		sprintf(strCount, "%d", masterls.numberOfDCs);
		sprintf(strProcessID, "%d", masterls.dc[DC_count].dcProcessID);

		strcpy(rem_dc_log, "DC- ");
                strcat(rem_dc_log, DC_count);
                strcat(rem_dc_log, " [");
                strcat(rem_dc_log, masterls.dc[DC_count].dcProcessID);
                strcat(rem_dc_log, "]");
                 //strcat(new_dc_log, dc_pid);
                strcat(new_dc_log, " removed from the master list - NON-RESPONSIVE");
                fprintf(log_stream, "%s", rem_dc_log);
                   break;
               }
            }
            else
            {
                masterls.msgQueueID = mid;
		localNumDCs++;
		masterls.numberOfDCs = localNumDCs;
		printf("numbofdc: %d\n", masterls.numberOfDCs);
                //pid t DCMessage.ID
                masterls.dc[DC_count].dcProcessID = (pid_t) incom_msg.machinePID; //what to do here?

		sprintf(strCount, "%d", masterls.numberOfDCs);
		sprintf(strProcessID, "%d", masterls.dc[DC_count].dcProcessID);

		strcpy(new_dc_log, "DC- ");
                strcat(new_dc_log, strCount);
                strcat(new_dc_log, " [");
                strcat(new_dc_log, strProcessID);
                strcat(new_dc_log, "]");
                strcat(new_dc_log, " added to the master list - NEW DC  - Status 0 (Everything is OOKAY)");
		printf("%s\n", new_dc_log);
                fprintf(log_stream, "%s\n", new_dc_log);
		fflush(log_stream);
                time(&rawtime);
	            timeinfo = localtime(&rawtime);
                masterls.dc[DC_count].lastTimeHeardFrom =  asctime(timeinfo);
                printf("\nThe number of DCs is %d\n", masterls.numberOfDCs);
            }

            start = clock();
            memset(new_dc_log,0,sizeof(new_dc_log)); 
            
            printf("%.2f\n", (double) (time(NULL) - start));

          sleep(LAST_SLEEP);
	}

	/* our server is done, so shut down the queue */

	    msgctl (mid, IPC_RMID, (struct msqid_ds *)NULL);


  // done with the message queue - so clean-up
    msgctl (mid, IPC_RMID, NULL);
    printf ("(SERVER) Message QUEUE has been removed\n");
    fflush (stdout);
    fclose(log_stream);

	return 0;
}


//
// FUNCTION : delay()
// DESCRIPTION : This function gets the delay value
// in milliseconds and simply performs the delay.
// This function was retrieved on November 13, 2020
// from https://c-for-dummies.com/blog/?p=69.
// PARAMETERS : int milliseconds - milliseconds
// RETURNS : NONE
//s
void delay(int milliseconds)
{
    long pause;
    clock_t now, then;

    pause = milliseconds * (CLOCKS_PER_SEC / 1000);
    now = then = clock();

    while ((now - then) < pause)
    {
        now = clock();
    }
}
