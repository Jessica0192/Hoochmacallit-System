/*
* FILE: dcSubFunctions.c
* PROJECT: A3
* PROGRAMMER: JESSICA SIM
* FIRST VERSION: 2021-03-11
* DESCRIPTION: This file contains several functions that is going to be called to help to send message to server
*/

#include "../inc/dc.h"


/*
* FUNCTION: getStatus
* PURPOSE: The function takes integer value of status and depends on the value, it returns the corresponding message
* INPUTS: int status - integer value of status
* RETURNS: char* - returns the corresponding message of status
*/
char* getStatus(int status)
{
  switch(status)
  {
	  case EVERYTHING_OK:
		return "Everything is OKAY";
	  case HYDRAULIC_FAIL:
		return "Hydraulic Pressure Failure";
	  case SAFETY_BTN_FAIL:
		return "Safety Button Failure";
	  case NO_RAW_MATERIAL:
		return "No Raw Material in the Process";
	  case OPERATING_TEMP_OUT_OF_RANGE:
	 	return "Operating Temparature Out of Range";
	  case OPERATOR_ERR:
		return "Operator Error";
	  case MACHINE_OFF_LINE:
		return "Machine is Off-line";
  }
}

/*
* FUNCTION: send_message
* PURPOSE: The function takes message id, pid, and message that will be sent to server. Using "msgsnd" function,
* sends the message with the passed message id.
* INPUTS: int mid - integer value of message id
	  pid_t pid - pid of current DC application
          char* msg - pointer to char array that contains message which will be sent to server
* RETURNS: int - success or failed
*/
int send_message (int mid, pid_t pid, char* msg)
{
	DCMessage sendMsg;
	int sizeofdata = sizeof (DCMessage) - sizeof (long);
        sendMsg.type = TYPE_SERVERMESSAGE;
	memset(sendMsg.msg,0, sizeof(sendMsg.msg));
	sendMsg.machinePID = pid;
	strcat(sendMsg.msg, msg);

	printf("machinPID: %d\n", sendMsg.machinePID);
	printf("msg: %s\n", sendMsg.msg);

	// send the message to server
	if(msgsnd (mid, (void *)&sendMsg, sizeofdata, 0) == -1)
	{
	   printf("Error in msgsnd\n");
	   return 1;
	}
  	return 0;
}

/*
* FUNCTION: createLog
* PURPOSE: The function takes status of the message, pid, and message that is sent to server. It stores a log message 
* that the specific "msg" is sent to server
* INPUTS: int status - integer value of status
	  pid_t pid - pid of current DC application
          char* msg - pointer to char array that contains message which will be sent to server
* RETURNS: int - success or failed
*/
int createLog(int status, pid_t pid, char* msg)
{
	int check = 0;
	char* dirname = "tmp";
	char* path = "tmp/dataCreator.log";
	FILE* ofp;

	struct stat st = {0};
	if (stat(dirname, &st) == -1) {
    		check = mkdir(dirname, 0700);
	}
	if (check == -1){
        	printf("Unable to create directory\n"); 
        	exit(1); 
	}

	if( access( path, F_OK ) == 0 ) {
	    	// file exists
		ofp = fopen(path, "a");
		if(ofp == NULL)
		{
			printf("Error on appending log file\n");
			return 1;
		}
	} else {
	    	// file doesn't exist
	    	ofp = fopen(path, "a");
		if(ofp == NULL)
		{
			printf("Error on creating log file\n");
			return 1;
		}
	}


	time_t t = time(NULL);
	struct tm tm = *localtime(&t);


	fprintf(ofp, "[%04d-%02d-%02d %02d:%02d:%02d] : DC [%d] - MSG SENT - Status %d (%s)\n",
	 tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, pid, status, msg);
	fclose(ofp);
}
