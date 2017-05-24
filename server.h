/************************************************************************
 * Name: Rohan Sharma                                                   *
 * Login: rsharma1                                                      *
 * Student Number: 639271                                               *
 * COMP30023 Computer Systems Project 2 - server/client for connect4    *
 *                                                                      *
 * server.h: A header file which defines the libraries, data structures *
 * and functional prototypes used by server.c                           *
 ***********************************************************************/

/*-------------------------- Libraries --------------------------------*/
#ifndef SERVER_H
#define SERVER_H

#include "work.h"
#include "string-helper-functions.c"
/*---------------------------------------------------------------------*/


/*------------------------- Data structures ---------------------------*/
pthread_mutex_t lock;
FILE *fp;
/* Client information (Passed into thread) */
typedef struct {
	int client_fd;
	struct sockaddr_in client_addr;
	struct sockaddr_in server_addr;
	int newsocket_fd;
} client_info_t;
/*---------------------------------------------------------------------*/

/*---------------------------- Constants ------------------------------*/
#define N_ARGS 2
#define TRUE 1
#define OUT_FILE "log.txt"
#define DIFFICULTY_LEN 4
#define SEED_LEN 32
#define SOLUTION_LEN 8
#define MAX_CLIENTS 100
#define MAX_WORK 10
/*---------------------------------------------------------------------*/

/*------------------------- Functional prototypes ---------------------*/
/* The work function for the thread.
 * Input: A pointer to the file descriptor used for communication.
 * Output: None, plays game with the client and logs the activity.
 */
void * work_function(void * params);
int isvalid(BYTE *target,BYTE* seedBYTE, BYTE* solutionBYTE);
void ctob(char* string, int stringlen, BYTE *number);
char getcharacter(int number);
int getval(BYTE character);
char * work(char *buffer, int bufferlen, BYTE *difficultyBYTE, BYTE *seedBYTE, BYTE *startBYTE);
void btoc(BYTE *number, int numberlen, char *string);
void getTarget(BYTE* target, BYTE* difficultyBYTE);
void receive_log(client_info_t client_info, char *msg);



/* Writes out a connection log to the file.
 * Input: Client info struct.
 * Output: None.
 */
void connection_log(client_info_t);
void pong_log(client_info_t);
void error_log(client_info_t, char*);
void okay_log(client_info_t);
void solu_log(client_info_t, char*);

/* Writes out move logs to the output file.
 * Input: Client info struct, (SERVER_MOVE|CLIENT_MOVE), move,
 * Output: None.
 */
void move_log(client_info_t, int, int);

/* Writes out the logs when the game ends.
 * Input: Client info struct, STATUS (Defined in server.h).
 * Output: None.
 */
void end_game_log(client_info_t, int);
/*---------------------------------------------------------------------*/
#endif
