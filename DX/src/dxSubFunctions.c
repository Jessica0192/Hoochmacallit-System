#include "../inc/dx.h"


int executeAction(int status, MasterList* masterls)
{
  key_t message_key = 0;
  time_t T = time(NULL);
  pid_t pid;
  int retVal=0;

  switch(status)
  {
	  case 0:
	  case 8:
	  case 19:
		//do nothing
		break;
	  case 1:
	  case 4:
	  case 11:
		//kill DC-01 if exists
		pid = masterls->dc[0].dcProcessID;
		if(masterls->numberOfDCs > 0)
		{
		   kill(pid, SIGKILL);
		}
		retVal=createLogMsgWOD(pid, status, 1, "TERMINATED");
		break;
	  case 2:
	  case 5:
	  case 15:
		//kill DC-03 if exists
		pid = masterls->dc[2].dcProcessID;
		if(masterls->numberOfDCs > 2)
		{
		   kill(pid, SIGKILL);
		}
		retVal=createLogMsgWOD(pid, status, 3, "TERMINATED");
		break;
	  case 3:
	  case 6:
	  case 13:
		//kill DC-02 if exists
		pid = masterls->dc[1].dcProcessID;
		if(masterls->numberOfDCs > 1)
		{
		   kill(pid, SIGKILL);
		}
		retVal=createLogMsgWOD(pid, status, 2, "TERMINATED");
		break;
	  case 10:
	  case 17:
		//delete message queue being used between DCs and DR
		message_key = ftok (".", 'M');
		msgctl (message_key, IPC_RMID, NULL);
  		printf ("(DX) Message QUEUE has been removed\n");
  		fflush (stdout);
		retVal=createLogMsgWOD(0, status, 0, "DX deleted the msgQ - the DR/DCs can't talk anymore - exiting");
		break;
	  case 7:
	 	//kill DC-04 if exists
		pid = masterls->dc[3].dcProcessID;
		if(masterls->numberOfDCs > 3)
		{
		   kill(pid, SIGKILL);
		}
		retVal=createLogMsgWOD(pid, status, 4, "TERMINATED");
		break;
	  case 9:
		//kill DC-05 if exists
		pid = masterls->dc[4].dcProcessID;
		if(masterls->numberOfDCs > 4)
		{
		   kill(pid, SIGKILL);
		}
		retVal=createLogMsgWOD(pid, status, 5, "TERMINATED");
		break;
	  case 12:
		//kill DC-06 if exists
		pid = masterls->dc[5].dcProcessID;
		if(masterls->numberOfDCs > 5)
		{
		   kill(pid, SIGKILL);
		}
		retVal=createLogMsgWOD(pid, status, 6, "TERMINATED");
		break;
	  case 14:
		//kill DC-07 if exists
		pid = masterls->dc[6].dcProcessID;
		if(masterls->numberOfDCs > 6)
		{
		   kill(pid, SIGKILL);
		}
		retVal=createLogMsgWOD(pid, status, 7, "TERMINATED");
		break;
	  case 16:
		//kill DC-08 if exists
		pid = masterls->dc[7].dcProcessID;
		if(masterls->numberOfDCs > 7)
		{
		   kill(pid, SIGKILL);
		}
		retVal=createLogMsgWOD(pid, status, 8, "TERMINATED");
		break;
	  case 18:
		//kill DC-09 if exists
		pid = masterls->dc[8].dcProcessID;
		if(masterls->numberOfDCs > 8)
		{
		   kill(pid, SIGKILL);
		}
		retVal=createLogMsgWOD(pid, status, 9, "TERMINATED");
		break;
	  case 20:
		//kill DC-10 if exists
		pid = masterls->dc[9].dcProcessID;
		if(masterls->numberOfDCs > 9)
		{
		   kill(pid, SIGKILL);
		}
		retVal=createLogMsgWOD(pid, status, 10, "TERMINATED");
		break;
  }
  if(retVal == 1)
  {
    return 1;
  }
  return 0;
}

int createLogMsgWOD(pid_t pid, int actionNum, int dcNum, char* msg)
{
  int check = 0;
  char* dirname = "tmp";
  char* path = "/tmp/dataCorruptor.log";
  char* fullMsg = NULL;
  FILE* ofp;

  struct stat st = {0};
  if (stat(dirname, &st) == -1) {
     	check = mkdir(dirname, 0700);
  }
  if (check == -1){
      	printf("Unable to create directory\n"); 
        exit(1); 
  }


  ofp = fopen(path, "a");
  if(ofp == NULL)
  {
    printf("Error on creating log file\n");
    return 1;
  }

  time_t t = time(NULL);
  struct tm T = *localtime(&t);
  
  if(actionNum == 10 || actionNum == 17)
  {
    fprintf(ofp, "[%04d-%02d-%02d %02d:%02d:%02d] : %s\n",
	 T.tm_year+1900, T.tm_mon+1, T.tm_mday, T.tm_hour, T.tm_min, T.tm_sec, msg);
  }
  else if(pid == 0 && actionNum == 0 && dcNum == 0)
  {
    fprintf(ofp, "[%04d-%02d-%02d %02d:%02d:%02d] : %s\n",
	 T.tm_year+1900, T.tm_mon+1, T.tm_mday, T.tm_hour, T.tm_min, T.tm_sec, msg);
  }
  else
  {
    fprintf(ofp, "[%04d-%02d-%02d %02d:%02d:%02d] : WOD Action %02d - DC-%02d [%d] %s\n",
	 T.tm_year+1900, T.tm_mon+1, T.tm_mday, T.tm_hour, T.tm_min, T.tm_sec, actionNum, dcNum, pid, msg);
  }
  
  return 0;
}
