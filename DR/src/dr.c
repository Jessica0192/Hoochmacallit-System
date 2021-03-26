
/*
* FILE          : dr.c
* PROJECT       : SENG2030 - Assignment #3
* PROGRAMMER    : Maria Malinina
* FIRST VERSION : 2020-03-14
* DESCRIPTION   :
* This file contains the data reader program. This program implements the IPC mechanism.
* It monitor for the incoming message queue for the varying statuses of the different 
* Hoochamacallit machines that are available (Data Creator - DC applications). DR keeps
* track of the number of different and active machines present in the system, we save them
* into the Master List - keeping track of each DC that is talking to DR and outputing it to
* the log file, which can be found in bin/tmp directory of the DR project directory. The log 
* file also contains the information about DC sending new message - "update" log message, DC
* going offline/being non-responsive and all DCs going offline. The DR application creates
* message queue and when it detects that all of its feeding machineshave gone off-line, 
* it frees up the message queue, releases its shared memory and terminates.
*/

#include "../inc/dr.h"

int main(void)
{
	MasterList* p;
	
	int mid; // message queue ID
	int sizeofdata;
	int continueToRun = 1;
	int DC_pids[MAX_DC_ROLES] = {0}; //local variable where we'll be saving DCs' pids.
	//and the array indexes will be their IDs
	time_t rawtime; //to keep track of time
    	struct tm* timeinfo;
 
    	int howManySec = 0;
	MasterList* msList;

	//variable where we'll be saving the ID of the DC to remove
	int removalID = 0;
	//a boolean variable that'll indicate if we currently need to 
	//remove some DC or not
	bool shouldRemove = false;
	//a string we'll use to output errors into the log file, in case
	//there are any
	char err_dc_log[255];

	//different status messages to be able to identify which status is the
	//message the DC just sent
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

    	key_t message_key; //the message key
    	int check = 0;

	//now, let's put the path of our log message
	char* dirname = "tmp";
	char* path = "tmp/dataMonitor.log";
	FILE* log_stream; //the FILE stream where we'll output log messages

	struct stat st = {0};
	if (stat(dirname, &st) == -1) {
    		check = mkdir(dirname, 0700);
	}
	if (check == -1){
		//error in creating the directory
        	exit(1); 
	}

	//check if everything's ok with appending
	//to the log file
	if( access( path, F_OK ) == 0 ) {
	    	// file exists
		log_stream = fopen(path, "a");
		if(log_stream == NULL)
		{//error in creating the path
			return 1;
		}
	} else {
	    	// file doesn't exist
	    	log_stream = fopen(path, "a");
		if(log_stream == NULL)
		{//error in creating the log file
			return 1;
		}
	}


	// get the unique token for the message queue (based on some agreed 
    	// upon "secret" information  
    	message_key = ftok ("../../", 'M');
    	if (message_key == -1) 
    	{ 
        	strcpy(err_dc_log,"Cannot create message key!\n");
        	fprintf (log_stream, "%s", err_dc_log);
        	return 1;
    	}

	// create the message queue
    	mid = msgget (message_key, IPC_CREAT | 0660);
    	if (mid == -1) 
    	{
        	strcpy(err_dc_log,"ERROR: Cannot create message queue\n");
        	fprintf (log_stream, "%s", err_dc_log);
        	return 2;
    	}

	//set the message queue id
	//msList->msgQueueID = mid;

    	//now create (or find a) shared mem obj
    	//the function will retunr -1 if the sys
    	//is unable to create the obj
    	int segment_id;

    	key_t shmkey; //shared memory key
    	int shmid;//shared memory id
    	char* shared_memory;
    	//struct shmid_ds shmbuffer;
    	int segment_size;
    	const int shared_segment_size = 0x6400;
    	/* Allocate a shared memory segment.  */

	// ftok to generate unique key 
    	shmkey = ftok(".", 16535); 
	if (shmkey == -1) 
	{ 
	  strcpy(err_dc_log,"Cannot allocate shared memory key\n");
          fprintf (log_stream, "%s", err_dc_log);
	  return 1;
	}

	if ((shmid = shmget (shmkey, sizeof (MasterList) - sizeof(long), 0)) == -1) 
	{

		/*
		 * now, let's create one (user/group read/write perms)
		 */

		shmid = shmget (shmkey, sizeof (MasterList)- sizeof(long), IPC_CREAT | 0660);//get the shared memory id
		if (shmid == -1) 
		{
		  strcpy(err_dc_log,"Cannot allocate a new shared memory!\n");
        	  fprintf (log_stream, "%s", err_dc_log);
		  return 2;
		}
	}
	//allocate enough space for the master list
	msList = (MasterList *)shmat (shmid, NULL, 0);
	if (msList == NULL) 
	{
	  strcpy(err_dc_log,"Cannot attach to shared memory!\n");
          fprintf (log_stream, "%s", err_dc_log);
	  return 3;
	}

  
	//sleep for 15 seconds before beginning the main
	//loop
    	sleep(FIRST_SLEEP);



    	// compute size of data portion of message
    	sizeofdata = sizeof (MasterList) - sizeof (long);
  	
    	int rc;	// return code from message processing

	//boolean variable we'll use to determine if the current DC talking to 
	//DR is an existing machine in our master list or not
   	bool existDCOrNot = false;
   	int localNumDCs = 0;	//counter to keep track of how many DCs we are currently talking to
   	char new_dc_log[255];	//string for new DC log message
	char upd_dc_log[255];	//string for update in DC's message log message
   	char rem_dc_log[255];	//string for DC removal log message
   	char strCount[5] = {0};		//string count of DCs when writing to file
   	char strProcessID[20] = {0};		//string processID when writing to file

	//set memory for the strings that'll contain log messages
   	memset(new_dc_log ,0, sizeof(new_dc_log));
   	memset(rem_dc_log ,0, sizeof(rem_dc_log));
	memset(upd_dc_log ,0, sizeof(upd_dc_log));

	//implement a structure object for the current incoming message
  	DCMessage incom_msg;

  	
	int index = 0;

  	//MAIN LOOP
    	while (1)
        {
		//set the memory for the string that'll contain the current talking DC's id
		//and the string the current talking DC's pid
	   	memset(strProcessID,0,sizeof(strProcessID));

           	//now, while we are waiting for any message, we calcuate (increase) the last time heard from
		//for each DC, as if we still haven't received any message - we need to be constantly increasing
		//each of the talking DCs' lastTimeHeardFrom property
		if (localNumDCs != 0)
		{
			while(1)
			{
				//while we are waiting for messages and increasing lastTimeHeardFrom, we are constantly checking for 
				//any available messages - we do this by constantly calling msgrcv() with IPC_NOWAIT, which means if
				//there is any message - we'll break out if this loop, if there is no message - we go to next lines 
				//to calculate DCs' lastTimeHeardFrom, otherwise, if the last arguement was 0 we would be stuck on that 
				//line without being able to handle the timing
				rc = msgrcv(mid, &incom_msg, sizeof (DCMessage) - sizeof (long),0 ,IPC_NOWAIT);
				if (rc != -1)
				{//if there is any message available - break out this loop and proceed with main loop	
					break;
				}
				//new time object to appropriately calculate the lastTimeHeardFrom
			 	time(&rawtime);
  
    				timeinfo = localtime(&rawtime);
		
				removalID = 0;
				shouldRemove = false;

				//check each of the MasterList's DCs to see if we should remove some
				//DC
				for (int i = 0; i < localNumDCs; i++)
				{	//we remove the DC if we haven't heard from it for more than 35 seconds
					if (msList->dc[i].lastTimeHeardFrom.time >35)
					{ 
						removalID = i;
						shouldRemove = true;
					}
				}

				//removal logic
				if (shouldRemove == true)
				{
					//firstly, decrement the number of DCs
					localNumDCs--;
		
					//removeLogic(localNumDCs, msList, removalID, strCount, strProcessID, DC_pids, rem_dc_log, log_stream);
					//and set it in the masterlist
					msList->numberOfDCs = localNumDCs;
					howManySec = 0; //reset
					
					//get the DC's id into the string object
					sprintf(strCount, "%d", removalID);

					//get the process id into the string object
					sprintf(strProcessID, "%d",DC_pids[removalID]);

					//log the removal message
					removeDC(strCount, strProcessID, rem_dc_log, log_stream, 0);
					
					//now call the function that will appropriately collapse the master list
					//and rearrange DC objects, as we've just removed a DC
					rearrange(removalID, (localNumDCs), msList, DC_pids);

					//check if there are any DCs left talking DR at all
					if (localNumDCs == 0)
					{
						char* allDCOffMsg = "All DCs have gone offline or terminated – DR TERMINATING\n";
						fprintf(log_stream, "%s", allDCOffMsg);
						break;
					}
			
               			}
				//set the index of the DC we need to update the lastTimeHeardFrom property in
				if (localNumDCs == 1) 
				{
					index = 0;
				}
				else
				{
					index = msList->numberOfDCs;
				}

				//update lastTimeHeardFrom again
				for (int i = 0; i < localNumDCs; i++)
				{
					msList->dc[i].lastTimeHeardFrom.time = ((timeinfo->tm_min * 60) + timeinfo->tm_sec) - ((msList->dc[i].lastTimeHeardFrom.minutes * 60) + msList->dc[i].lastTimeHeardFrom.seconds);
					if (msList->dc[i].lastTimeHeardFrom.time > 35)
					{
						howManySec = 35; 
						break;
					}
				}
		
				//set the howManySec variable to indicate that we should remove some DC
				if (((timeinfo->tm_min * 60) + timeinfo->tm_sec) - ((msList->dc[index].lastTimeHeardFrom.minutes * 60) + msList->dc[index].lastTimeHeardFrom.seconds) >35)
				{
					howManySec = 35;
					break;
				}
		
			}	
		}
		else //otherwise we are waiting for the next available message
		{
			rc = msgrcv(mid, &incom_msg, sizeof (DCMessage) - sizeof (long),0 ,0);
		}
		 
		//get the current time by instantiating a new time object
		time(&rawtime);
    		timeinfo = localtime(&rawtime);
        	int new_size = sizeof (DCMessage) - sizeof (long);

	
		time(&rawtime);
	  
	    	timeinfo = localtime(&rawtime);
	    	asctime(timeinfo);
		//get the index of the current talking DC
		for (int i = 0; i < localNumDCs; i++)
		{
			if (incom_msg.machinePID == DC_pids[i])
			{
				index = i;
			}
		} //again, update the lastTimeHeardFrom property for the current talking DC
	    	msList->dc[index].lastTimeHeardFrom.hours = timeinfo->tm_hour;
	    	msList->dc[index].lastTimeHeardFrom.minutes = timeinfo->tm_min;
	    	msList->dc[index].lastTimeHeardFrom.seconds = timeinfo->tm_sec;
	

		//check if there was any error while receiving the message	
		if (howManySec != 35)
		{
			if (rc == -1) 
			{
				strcpy(err_dc_log,"An error occured while trying to receive a message...\n");
        			fprintf(log_stream, "%s", err_dc_log);
		       		break;
			}
		}
           


		int removalID = 0;
		bool shouldRemove = false;
		//get the removalID and set the boolean shouldRemove flag to true!
		for (int i = 0; i < localNumDCs; i++)
		{
			if (msList->dc[i].lastTimeHeardFrom.time >35)
			{ 
				removalID = i;
				shouldRemove = true;
			}
		}

		//go to the removal logic if the removal flag was set
		if (shouldRemove == true)
		{
			//decrease the number of DCs that are talking to DR
			localNumDCs--;
		
			//set the number of DCs in the master list
			msList->numberOfDCs = localNumDCs;
			howManySec = 0; //reset
			//set the removal ID to the string object that we'll use to 
			sprintf(strCount, "%d", removalID); //output the DC's id in the log file

			//and set the process id string object
			sprintf(strProcessID, "%d",DC_pids[removalID]);

			//log the DC's removal
			removeDC(strCount, strProcessID, rem_dc_log, log_stream, 0);


			//collapse - rearrange the master list, as we've just deleted some DC from the list
			 rearrange(removalID, (localNumDCs), msList, DC_pids);

			//check if any DC machines are still talking to DR
			if (localNumDCs == 0)
			{
				char* allDCOffMsg = "All DCs have gone offline or terminated – DR TERMINATING";
				fprintf(log_stream, "%s", allDCOffMsg);
				break;
			}
		  }
		  else
		  {

			//set the string object that will indicate the DC's id
			sprintf(strCount, "%d", msList->numberOfDCs);

			int iterator = 0;
			if (localNumDCs != 0){
				iterator = localNumDCs;	
			//find out if the DC that is talking to us is already on the master list
			//or it is a new one
			for (int i = 0; i < iterator; i ++)
			{
				if ((pid_t)msList->dc[i].dcProcessID == (pid_t)incom_msg.machinePID)
				{ //if DC is already on the list - set the flag to true
					existDCOrNot = true;
				}
			}
		}

		//if the DC is a new machine - we need to log it
		if (existDCOrNot == false)
		{
			//set the time property to 0 as we are "hearing from the DC" right now
			msList->dc[localNumDCs].lastTimeHeardFrom.time = 0;

			//set the PID in the master list
			msList->dc[localNumDCs].dcProcessID = (pid_t) incom_msg.machinePID;
			
			//set the string object indicating the process id of the machine
			sprintf(strProcessID, "%d", msList->dc[localNumDCs].dcProcessID);

			//and set the local variable with DC pids
			DC_pids[localNumDCs] = msList->dc[localNumDCs].dcProcessID;

			//increase the number of DCs in the master list
			localNumDCs++;

			//we need to make sure that no more than 10 DCs can talk to us
			if (localNumDCs <= MAX_DC_ROLES)
			{
				msList->numberOfDCs = localNumDCs;
				time_t t = time(NULL);
			    	struct tm tm = *localtime(&t);
				fprintf(log_stream, "["); //NEW TIME
				fprintf(log_stream, "%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
				fprintf(log_stream, "] ");
				strcpy(new_dc_log, "DC- ");
				sprintf(strCount, "%d", (localNumDCs-1));
				strcat(new_dc_log, strCount); //DC's id
				strcat(new_dc_log, " [");
				strcat(new_dc_log, strProcessID); //DC's pid
				strcat(new_dc_log, "]");
				strcat(new_dc_log, " added to the master list - NEW DC  - Status 0 (Everything is OKAY)");
				fprintf(log_stream, "%s\n", new_dc_log);
				fflush(log_stream);
			

				time(&rawtime);
	  
			    	timeinfo = localtime(&rawtime);
			    	asctime(timeinfo);

			
			    	msList->dc[(localNumDCs-1)].lastTimeHeardFrom.hours = timeinfo->tm_hour;
			    	msList->dc[(localNumDCs-1)].lastTimeHeardFrom.minutes = timeinfo->tm_min;
			    	msList->dc[(localNumDCs-1)].lastTimeHeardFrom.seconds = timeinfo->tm_sec;
			}
			else
			{	//if there are more than 10 DCs, we simply ignore them, 
				localNumDCs = MAX_DC_ROLES; //allowing only 10 machines to talk to us
			}
		}
		else if (rc >= 0)//check if the return code of msgget() indicates we received a message
		{
			int cur_dc_id = 0;
			
			//searching for the current DC's id 
			for (int i = 0; i < MAX_DC_ROLES; i++)
			{
				if (DC_pids[i] == incom_msg.machinePID)
				{//set the current DC's id
					cur_dc_id = i;
					removalID = i;
				}
			}
			//copy the current DC's pid
			sprintf(strProcessID, "%d", incom_msg.machinePID);
			
			//new time object to calculate the timing appropriately
			time_t t = time(NULL);
		    	struct tm tm = *localtime(&t);

			sprintf(strCount, "%d", (cur_dc_id)); //put the DC's ID in the string object
			strcat(upd_dc_log, strProcessID); //put the DC's process ID in the string object
			msList->dc[cur_dc_id].lastTimeHeardFrom.time = 0; //set lastTimeHeardFrom to 0 
			//since we are now talking to that DC
			
			//check which status the current message we are receiving has and set the appropriate status
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
			
			//if the status is not 6 - then we need to log message about the DC's update
			if (strcmp(status, "06")!=0)
			{	//produce a log message indicating we heard an update from the DC, indicating the status and the message
				fprintf(log_stream, "["); 
				fprintf(log_stream, "%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
				fprintf(log_stream, "] ");
				strcpy(upd_dc_log, "DC- ");
				sprintf(strCount, "%d", (cur_dc_id));
				strcat(upd_dc_log, strCount); //DC's id
				strcat(upd_dc_log, " [");
				strcat(upd_dc_log, strProcessID); //DC's pid
				strcat(upd_dc_log, "]");
				strcat(upd_dc_log, " updated in the master list - MSG RECEIVED  - Status ");
				strcat(upd_dc_log, status);
				strcat(upd_dc_log, " ");
				strcat(upd_dc_log, "(");
				strcat(upd_dc_log, incom_msg.msg);
				strcat(upd_dc_log, ")");
				
				//produce log message, log it to the log file
				fprintf(log_stream, "%s\n", upd_dc_log);
				strcpy(upd_dc_log,"");
				fflush(log_stream);
			}
			else
			{ //if the status - we need to remove the DC from the master list
				localNumDCs--;
				msList->numberOfDCs = localNumDCs;
				removeDC(strCount, strProcessID, upd_dc_log, log_stream, 6); //produce the removal log
				//message about the DC going offline
				
				//after the removal we need to rearrange the master list
				rearrange(removalID, (localNumDCs), msList, DC_pids);
				
				//check if there are any DCs talking to us left
				if (localNumDCs == 0)
				{	//if yes - log a message about DR terminating and terminate
					char* allDCOffMsg = "All DCs have gone offline or terminated – DR TERMINATING\n";
					fprintf(log_stream, "%s", allDCOffMsg);
					break;
				}
			}
			//new time object to appropriately calculate the time
			time(&rawtime);
		    	timeinfo = localtime(&rawtime);
		    	asctime(timeinfo);

			//set the time properties of the current DC
		    	msList->dc[cur_dc_id].lastTimeHeardFrom.hours = timeinfo->tm_hour;
		    	msList->dc[cur_dc_id].lastTimeHeardFrom.minutes = timeinfo->tm_min;
		    	msList->dc[cur_dc_id].lastTimeHeardFrom.seconds = timeinfo->tm_sec;

			
		}
                time(&rawtime);
	        timeinfo = localtime(&rawtime);

		//reset the flags indicating if the current DC is a new one or not and if we
		//should remove some DC
		existDCOrNot = false;
		shouldRemove = false;
            }
            
	//lastly, sleep for 1.5s	
	sleep(LAST_SLEEP);
	}

	/* our server is done, so shut down the queue */
	msgctl (mid, IPC_RMID, (struct msqid_ds *)NULL);
	
	/* Detach the shared memory segment.  */
	shmctl(shmid, IPC_RMID, NULL);

  	//close the log file stream
   	fclose(log_stream);

	return 0;
}


