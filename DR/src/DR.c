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
#include <stdbool.h>

#pragma warning (disable: 4996)
#include "../inc/data_reader.h"


int main(void)
{
	int mid; // message queue ID
	int sizeofdata;
	int continueToRun = 1;
	int DC_pids[10] = {0};
	time_t rawtime;
    	struct tm* timeinfo;
	int cur_min = 0;
    	int cur_sec = 0;
    	int howManySec = 0;

	char status0msg[100] = {0};
	strcpy(status0msg, "Everything is OKAY");
	char status1msg[100] = {0};
	strcpy(status1msg, "Hydraulic Pressure Failure");
	char status2msg[100] = {0};
	strcpy(status2msg, "Safety Button Failure");
	char status3msg[100] = {0};
	strcpy(status3msg, "No Raw Material in the Process");
	char status4msg[100] = {0};
	strcpy(status4msg, "Operating Temparature Out of Range");
	char status5msg[100] = {0};
	strcpy(status5msg, "Operator Error");
	char status6msg[100] = {0};
	strcpy(status6msg, "Machine is Off-line");
	char status[10] = {0};
   	//where we'll put address of shared memory
   	//cpmst *addr;

    	key_t message_key;
    	MasterList masterls;
  	//  MasterList response;
    	FILE* log_stream;

    	if ((log_stream = fopen("dataMonitor.log", "w+")) == NULL) {
            fprintf(stderr, "%s: %s\n", "Impossible to create a file", "server_log.txt");
	    printf("Error opening file\n");
            return -1;
        }

    	struct stat st; 
    	int ret = stat("dataMonitor.log", &st);
     	if(ret != 0) {
         return -1;
    	}   

 	int mode = W_OK;
    	if(access("dataMonitor.log", mode) ==0) {
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

    	key_t shmkey;
    	int shmid;
    	MasterList* msList;

    	char bogus;
    	char* shared_memory;
    	//struct shmid_ds shmbuffer;
    	int segment_size;
    	const int shared_segment_size = 0x6400;
    	/* Allocate a shared memory segment.  */

	// ftok to generate unique key 
    	shmkey = ftok(".", 16535); 
	if (shmkey == -1) 
	{ 
	  printf ("(PRODUCER) Cannot allocate key\n");
	  return 1;
	}
	printf("\nIPCREAT IS %d\n", IPC_CREAT);


	printf("shmkey: %d\n", shmkey);
	if ((shmid = shmget (shmkey, sizeof (MasterList), 0)) == -1) 
	{

		/*
		 * nope, let's create one (user/group read/write perms)
		 */

		printf ("(PRODUCER) No Shared-Memory currently available - so create!\n");
		shmid = shmget (shmid, sizeof (MasterList), IPC_CREAT | 0660);
		//address of the shared memory
		//addr = shmat(shmid,NULL,0);
		if (shmid == -1) 
		{
		  printf ("(PRODUCER) Cannot allocate a new memory!\n");
		  return 2;
		}
	}

	printf ("(PRODUCER) Our Shared-Memory ID is %d\n", shmid);

	msList = (MasterList *)shmat (shmid, NULL, 0);
	if (msList == NULL) 
	{
	  printf ("(PRODUCER) Cannot attach to shared memory!\n");
	  return 3;
	}

    	/* Determine the segment's size. */
    	printf("Message queue id is %d", mid);
  
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
   	bool existDCOrNot = false;
   	int DC_count = 0;
   	int localNumDCs = 0;
   	char new_dc_log[255];
	char upd_dc_log[255];
   	char rem_dc_log[255];
   	char strCount[5] = {0};		//string count of DCs when writing to file
   	char strProcessID[20] = {0};		//string processID when writing to file
   	memset(new_dc_log ,0, sizeof(new_dc_log));
   	memset(rem_dc_log ,0, sizeof(rem_dc_log));
  	// time_t strart = time(NULL);
  	DCMessage incom_msg;

  	//time_t rawtime;
  	//struct tm* timeinfo;
  	//time_t start_t, end_t;
  	//double diff_t;
	int index = 0;

  	//MAIN LOOP
    	while (1)
        {
	//check, compare PIDs to check if the current DC is existing one or not
	//->to be able to calculate the time
	//after checkin PID, update the log
	//check the yellow thing to "shift" up if the DC 0 for example goes offline
		memset(strCount, 0, sizeof(strCount));
	   	memset(strProcessID,0,sizeof(strProcessID));

           	printf ("(SERVER) Waiting for a message ...\n");
	

	if (localNumDCs != 0){
	while(1)
	{
		rc = msgrcv(mid, &incom_msg, sizeof (DCMessage) - sizeof (long),0 ,IPC_NOWAIT);
		if (rc != -1)
		{break;}
	 	time(&rawtime);
  
    		timeinfo = localtime(&rawtime);
		printf("\MINUTES RN %d SECONDS RN %d\n", timeinfo->tm_min, timeinfo->tm_sec);
		
		
		if (localNumDCs == 1) 
		{
			index = 0;
		}else{
			index = masterls.numberOfDCs;
		}
		printf("\nMINUTES from DC %d SECONDS from DC %d\n", masterls.dc[index].lastTimeHeardFrom.minutes, masterls.dc[index].lastTimeHeardFrom.seconds);
		if (((timeinfo->tm_min * 60) + timeinfo->tm_sec) - ((masterls.dc[index].lastTimeHeardFrom.minutes * 60) + masterls.dc[index].lastTimeHeardFrom.seconds) >35)
		{printf("\n35 SECONDS PASSED COMMON!\n"); howManySec = 35; break;}
		
	}}else {rc = msgrcv(mid, &incom_msg, sizeof (DCMessage) - sizeof (long),0 ,0);}
	  
             //START TIME HERE

		 time(&rawtime);
 
    	timeinfo = localtime(&rawtime);
	cur_min = timeinfo->tm_min;
    	cur_sec = timeinfo->tm_sec;
                // compute size of data portion of message //IT WAS MASTERLIST
        int new_size = sizeof (DCMessage) - sizeof (long);

	
	  
           // rc = msgrcv (mid, (void *)&msg, sizeof (DCInfo), 0, 0); //DCMessage
	//if (rc == -1)
	//{rc = msgrcv(mid, &incom_msg, new_size,0 ,0);}
          //this is the right one rc = msgrcv(mid, &incom_msg, new_size,0 ,0);
	//		END TIME HERE
	time(&rawtime);
  
    	timeinfo = localtime(&rawtime);
    	asctime(timeinfo);
    	masterls.dc[index].lastTimeHeardFrom.hours = timeinfo->tm_hour;
	printf("\nHOURS IS %d\n", masterls.dc[index].lastTimeHeardFrom.hours);
    	masterls.dc[index].lastTimeHeardFrom.minutes = timeinfo->tm_min;
	printf("\nMINUTES IS %d\n", masterls.dc[index].lastTimeHeardFrom.minutes);
    	masterls.dc[index].lastTimeHeardFrom.seconds = timeinfo->tm_sec;
	printf("\nSECONDS IS %d\n", masterls.dc[index].lastTimeHeardFrom.seconds);
    	printf("\nHERE %d\n", ((timeinfo->tm_min * 60) + timeinfo->tm_sec));
   	printf("\nAND HERE %d\n", ((cur_min * 60) + cur_sec));
    	printf("\nTHE ANSWER IS %d\n", howManySec);
	printf("incom_msg pid: %d\n", incom_msg.machinePID);

		
	//NEW THING - Now we need to save the current DC with all its features to 			//the master list
	//msList->dc[localNumDCs].dcProcessID = incom_msg.machinePID;
	//printf("\nADDED %d MACHINE TO THE MASTER LIST\n", msList->dc[localNumDCs].dcProcessID);
	//NEW ADDING LAST TIME HEARD FROM
	//masterls.dc[localNumDCs].lastTimeHeardFrom.seconds += howManySec;
	printf("\nLAST TIME HEARD FROM DC %d is %d\n", localNumDCs, howManySec);
	printf("incom_msg msg: %s\n", incom_msg.msg);
	printf("rc: %d\n", rc);
	printf("\nhow many sec is %d\n", howManySec);
	if (howManySec != 35){
        	if (rc == -1) 
        	{
        		printf("(SERVER) Error occured while trying to receive a message...");
               		break;
		}
	}
           
        if (howManySec == 35)
        {
		
		
		
		localNumDCs--;
		
			

		
                masterls.numberOfDCs = localNumDCs;
                howManySec = 0; //reset
		
		sprintf(strCount, "%d", masterls.numberOfDCs);
		//I CHANGED IT RNsprintf(strProcessID, "%d", incom_msg.machinePID);// masterls.dc[DC_count].dcProcessID);

		sprintf(strProcessID, "%d",DC_pids[masterls.numberOfDCs]);
		time_t t = time(NULL);
	    	struct tm tm = *localtime(&t);
	    	printf("now: %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
		fprintf(log_stream, "["); //NEW TIME
		fprintf(log_stream, "%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
		fprintf(log_stream, "] ");
		strcpy(rem_dc_log, "DC- ");
                strcat(rem_dc_log, strCount);
                strcat(rem_dc_log, " [");
                strcat(rem_dc_log, strProcessID);
                strcat(rem_dc_log, "]");
             //   strcat(new_dc_log, dc_pid);
                strcat(rem_dc_log, " removed from the master list - NON-RESPONSIVE");
                fprintf(log_stream, "%s", rem_dc_log);
		fprintf(log_stream, "\n");
		//check if there are any machines left at all
		if (localNumDCs == 0)
		{
			char* allDCOffMsg = "All DCs have gone offline or terminated – DR TERMINATING";
			fprintf(log_stream, "%s", allDCOffMsg);
			break;
		}
			
               }
            //}
            else
            {
                masterls.msgQueueID = mid;
		//COMMENTED OUT localNumDCs++;
		//commented out masterls.numberOfDCs = localNumDCs;
		printf("numbofdc: %d\n", masterls.numberOfDCs);
                //pid t DCMessage.ID
               //COMMENTED OUT THIS JUST NOW 12 08 AM 20 MARCH masterls.dc[DC_count].dcProcessID = (pid_t) incom_msg.machinePID; //what to do here?

		sprintf(strCount, "%d", masterls.numberOfDCs);
		//NEW COMMENTED OUT sprintf(strProcessID, "%d", masterls.dc[DC_count].dcProcessID);
		printf("\nTHE DC COUNT IS %d and numbofdc is %d\n", DC_count, masterls.numberOfDCs);

		//NEW THING
		if (masterls.numberOfDCs != 0){
			for (int i = 0; i < localNumDCs; i ++)
			{
				printf("\nmasterls.dc[i].dcProcessID is %d\n", masterls.dc[i].dcProcessID);
				printf("\nincom_msg.machinePID is %d\n", incom_msg.machinePID);
				if ((pid_t)masterls.dc[i].dcProcessID == (pid_t)incom_msg.machinePID)
				{
					existDCOrNot = true;
				}
			}
		}

		if (existDCOrNot == true){
		//existDCOrNot = false; //WAIT WHAT
		printf("\nFOR THIS %d PROCESS ID, the exist or not is TRUE\n", masterls.dc[localNumDCs].dcProcessID);}
		else
		{
			printf("\nFOR THIS %d PROCESS ID, the exist or not is FALSE\n", masterls.dc[localNumDCs].dcProcessID);
		}

		//NEW THING - THERE WAS NO IF STATEMENT
		if (existDCOrNot == false)
		{
			//sprintf(strProcessID, "%f", incom_msg.machinePID);//NEW - ADDED LINE HERE
			//LINE BELOW CHANGED FROM DC COUNT TO LOCALNUM
			masterls.dc[localNumDCs].dcProcessID = (pid_t) incom_msg.machinePID;//NEW ADDED LLINE'
			//LINE BELOW CHANGED FROM DC COUNT TO LOCALNUM
			sprintf(strProcessID, "%d", masterls.dc[localNumDCs].dcProcessID);//NEW ADDED LINE
			DC_pids[localNumDCs] = masterls.dc[localNumDCs].dcProcessID;

			printf("\nLOCAL NUM OF DCS BEFORE INCREMENTING %d\n", localNumDCs);
			localNumDCs++;
			printf("\nLOCAL NUM OF DCS AFTER INCREMENTING %d\n", localNumDCs);
			masterls.numberOfDCs = localNumDCs;
			time_t t = time(NULL);
		    	struct tm tm = *localtime(&t);
		    	printf("now: %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
			fprintf(log_stream, "["); //NEW TIME
			fprintf(log_stream, "%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
			fprintf(log_stream, "] ");
			strcpy(new_dc_log, "DC- ");
			sprintf(strCount, "%d", (localNumDCs-1));
		        strcat(new_dc_log, strCount); //was strCount
		        strcat(new_dc_log, " [");
		        strcat(new_dc_log, strProcessID); //strProcessID
		        strcat(new_dc_log, "]");
		        strcat(new_dc_log, " added to the master list - NEW DC  - Status 0 (Everything is OKAY)");
			printf("%s\n", new_dc_log);
		        fprintf(log_stream, "%s\n", new_dc_log);
			fflush(log_stream);
		}
		else //NEW ELSE
		{
			masterls.dc[localNumDCs].dcProcessID = (pid_t) incom_msg.machinePID;//NEW ADDED LLINE'
			int cur_dc_id = 0;
			//searching for the current DC's id 
			for (int i = 0; i < 10; i++)
			{
				if (DC_pids[i] == incom_msg.machinePID)
				{
					cur_dc_id = i;
				}
			}
			//LINE BELOW CHANGED FROM DC COUNT TO LOCALNUM
			sprintf(strProcessID, "%d", masterls.dc[localNumDCs].dcProcessID);//NEW ADDED LINE
			//DC_pids[localNumDCs] = masterls.dc[localNumDCs].dcProcessID;

			printf("\nLOCAL NUM OF DCS BEFORE INCREMENTING %d\n", localNumDCs);
			//localNumDCs++;
			printf("\nLOCAL NUM OF DCS AFTER INCREMENTING %d\n", localNumDCs);
			masterls.numberOfDCs = localNumDCs;
			time_t t = time(NULL);
		    	struct tm tm = *localtime(&t);
		    	printf("now: %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
			fprintf(log_stream, "["); //NEW TIME
			fprintf(log_stream, "%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
			fprintf(log_stream, "] ");
			strcpy(upd_dc_log, "DC- ");
			sprintf(strCount, "%d", (cur_dc_id));
		        strcat(upd_dc_log, strCount); //was strCount
		        strcat(upd_dc_log, " [");
		        strcat(upd_dc_log, strProcessID); //strProcessID
		        strcat(upd_dc_log, "]");
		        strcat(upd_dc_log, " updated in the master list - MSG RECEIVED  - Status ");
			if (strcmp(incom_msg.msg, status0msg) == 0)
			{
				strcpy(status, "00");
			}
			else if (strcmp(incom_msg.msg, status1msg) == 0)
			{
				strcpy(status, "01");
			}
			else if (strcmp(incom_msg.msg, status2msg) == 0)
			{
				strcpy(status, "02");
			}
			else if (strcmp(incom_msg.msg, status3msg) == 0)
			{
				strcpy(status, "03");
			}
			else if (strcmp(incom_msg.msg, status4msg) == 0)
			{
				strcpy(status, "04");
			}
			else if (strcmp(incom_msg.msg, status5msg) == 0)
			{
				strcpy(status, "05");
			}
			else
			{
				strcpy(status, "06");
			}
			strcat(upd_dc_log, status);
			strcat(upd_dc_log, " ");
			strcat(upd_dc_log, "(");
			strcat(upd_dc_log, incom_msg.msg);
			strcat(upd_dc_log, ")");
			printf("%s\n", upd_dc_log);
		        fprintf(log_stream, "%s\n", upd_dc_log);
			fflush(log_stream);
		}
                time(&rawtime);
	        timeinfo = localtime(&rawtime);
               //COMMENTED OUT masterls.dc[DC_count].lastTimeHeardFrom =  asctime(timeinfo);
                printf("\nThe number of DCs is %d\n", masterls.numberOfDCs);
		existDCOrNot = false;//NEW
            }//NEW SECTION FOR UPD MSG
	  //  else
	   // {
		
	//    }

            //start = clock();
            memset(new_dc_log,0,sizeof(new_dc_log)); 
            
            printf("%.2f\n", (double) (time(NULL) - start));

            sleep(LAST_SLEEP);

		//existDCOrNot = false;//NEW
	}

	/* our server is done, so shut down the queue */

	    msgctl (mid, IPC_RMID, (struct msqid_ds *)NULL);
	/* Detach the shared memory segment.  */
   	//shmdt(addr);
	shmctl(shmid, IPC_RMID, NULL);

  	// done with the message queue - so clean-up
   	// msgctl (mid, IPC_RMID, NULL);
   	printf ("(SERVER) Message QUEUE has been removed\n");
    	fflush (stdout);
   	fclose(log_stream);

	return 0;
}


