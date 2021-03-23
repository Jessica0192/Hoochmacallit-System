#define FIRST_SLEEP 15
#define LAST_SLEEP 1.5
#define TYPE_SERVERMESSAGE		1
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

#define MAX_DC_ROLES 10 

//key_t ftok(char* path, int id);
int msgget(key_t key, int msgFlag);
//key_t ftok(char* path, int id);
int msgget(key_t key, int msgFlag);
//int shmget(key_t key, int memSize, int memFLag);
void delay(int milliseconds);
