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
	sendMsg.machinePID = pid;
	sendMsg.msg = msg;

	// send the message to server
	if(msgsnd (mid, (void *)&sendMsg, sizeofdata, 0) == -1)
	{
	   printf("Error in msgsnd\n");
	   return 1;
	}
  	return 0;
}
