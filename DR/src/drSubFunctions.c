/*
* FILE          : drSubFunctions.c
* PROJECT       : PROG2121 - Assignment #3
* PROGRAMMER    : Maria Malinina
* FIRST VERSION : 2020-03-14
* DESCRIPTION   :
* This file contains the functions that the data reader - dr.c program - uses.
* Here, we have a function removeDC that logs the DC removal message into the log 
* file, rearrange function that rearranges the master list after DC removal and
* removeLogic function that actually removes the DC from the master list.
*/

#include "../inc/dr.h"

//
// METHOD      : removeDC()
// DESCRIPTION : When we need to remove DC after it being either non-responsive or sending a message
// with a status of 6 - we need to log a message about that to the log file. In this function, we pro-
// duce that message and log it.
// PARAMETERS  : char* strCount, the DC's id as a string object
//		 char*strProcessID, the DC's pid as a string object
//		 char* rem_dc_log, the string object that'll contain the whole message
//		 FILE* log_stream, the file stream where we'll output the message to
//		 int status, message status
// RETURNS     : NONE
//
void removeDC(char* strCount, char*strProcessID, char* rem_dc_log, FILE* log_stream, int status)
{
		//new time object because we need to indicate the time while logging the message
		time_t t = time(NULL);
	    	struct tm tm = *localtime(&t);
	   
		//produce the log message about DC removal
		fprintf(log_stream, "["); //NEW TIME
		fprintf(log_stream, "%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
		fprintf(log_stream, "] ");
		strcpy(rem_dc_log, "DC- ");
                strcat(rem_dc_log, strCount);
                strcat(rem_dc_log, " [");
                strcat(rem_dc_log, strProcessID);
                strcat(rem_dc_log, "]");
            
		//check if the message status was 6 - if yes log a message about DC going offline. if no - 
		//log a message about the DC being non-responsive
		if (status == 6)
		{
			strcat(rem_dc_log, " has gone OFFLINE - removing from the master list");
		}
		else
		{
                	strcat(rem_dc_log, " removed from the master list - NON-RESPONSIVE");
		}

		//output the message to the log file
                fprintf(log_stream, "%s", rem_dc_log);
		fprintf(log_stream, "\n");
}


//
// METHOD      : rearrange()
// DESCRIPTION : This function rearranges the master list after the DC removal.
// PARAMETERS  : int removalID, the DC's id
//		 int localNumDCs, how many DCs we currently have
//		 MasterList* msList, the master list
//		 int DC_pids[], local (to data reader) variable with DC PIds
// RETURNS     : NONE
//
void rearrange(int removalID, int localNumDCs, MasterList* msList, int DC_pids[])
{
	//relocate the DCs in the master list after the removal
	for(int j = removalID; j < localNumDCs; j++ )
	{
		DC_pids[j] = DC_pids[j + 1];//as well as the local array containing the DC PIDs
		msList->dc[j].dcProcessID = msList->dc[j + 1].dcProcessID;
		msList->dc[j].lastTimeHeardFrom.time = msList->dc[j + 1].lastTimeHeardFrom.time;
		msList->dc[j].lastTimeHeardFrom.hours = msList->dc[j + 1].lastTimeHeardFrom.hours;
		msList->dc[j].lastTimeHeardFrom.minutes = msList->dc[j + 1].lastTimeHeardFrom.minutes;
		msList->dc[j].lastTimeHeardFrom.seconds = msList->dc[j + 1].lastTimeHeardFrom.seconds;
	}
	//and clear out the last element because now there is 1 element less than it was before
	DC_pids[localNumDCs] = 0;
	msList->dc[localNumDCs].dcProcessID = 0;

}

//
// METHOD      : removeLogic()
// DESCRIPTION : This function rearranges the master list after the DC removal.
// PARAMETERS  : int localNumDCs, how many DCs we currently have
//		 MasterList* msList, the master list
//		 int removalID, the DC's id
//		 char* strCount, the DC's id as a string object
//		 char* strProcessID, the DC's pid as a string object
//		 int DC_pids[], local (to data reader) variable with DC PIds
//		 char* rem_dc_log, the string object that'll contain the final log message
//		 FILE* log_stream, file stream for the log file where we'll output the message
// RETURNS     : NONE
//
void removeLogic(int localNumDCs, MasterList* msList, int removalID, char* strCount, char* strProcessID, int DC_pids[], char* rem_dc_log, FILE* log_stream)
{

	//firstly, decrement the number of DCs
	localNumDCs--;
		
	//and set it in the masterlist
	msList->numberOfDCs = localNumDCs;
	
	//get the DC's id into the string object
	sprintf(strCount, "%d", removalID + 1);
			
	//get the process id into the string object
	sprintf(strProcessID, "%d",DC_pids[removalID]);

	//log the removal message
	removeDC(strCount, strProcessID, rem_dc_log, log_stream, 0);

	//now call the function that will appropriately collapse the master list
	//and rearrange DC objects, as we've just removed a DC
	rearrange(removalID, (localNumDCs), msList, DC_pids);
	
}
