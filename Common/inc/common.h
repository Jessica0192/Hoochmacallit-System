#define MAX_DC_ROLES 10

typedef struct  
{
	// the following is a requriement of UNIX/Linux
	long type;

	// now the specifics of our message
	pid_t machinePID;
	char msg[100];
	
} DCMessage;


typedef struct  
{
	int hours;
	int minutes;
	int seconds;
	int time;
	
} last_time;

typedef struct  
{
	// the following is a requriement of UNIX/Linux
	long type;

	// now the specifics of our message
	pid_t dcProcessID;
	last_time lastTimeHeardFrom;	//to calculate difference use "double difftime ( time_t time2, time_t time1 ); " 
	
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

