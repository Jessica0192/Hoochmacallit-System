#include "../inc/dc.h"

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

int createLog(int status, pid_t pid, char* msg, struct tm T)
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



	fprintf(ofp, "[%04d-%02d-%02d %02d:%02d:%02d] : DC [%d] - MSG SENT - Status %d (%s)\n",
	 T.tm_year+1900, T.tm_mon+1, T.tm_mday, T.tm_hour, T.tm_min, T.tm_sec, pid, status, msg);
	fclose(ofp);
}
