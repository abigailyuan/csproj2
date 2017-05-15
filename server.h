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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <pthread.h>
#include <time.h>
#include <arpa/inet.h>
#include "crypto/sha256.h"
#include "uint256.h"
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
#define MAX_CLIENTS 1000
/*---------------------------------------------------------------------*/

/*------------------------- Functional prototypes ---------------------*/
/* The work function for the thread.
 * Input: A pointer to the file descriptor used for communication.
 * Output: None, plays game with the client and logs the activity.
 */
void * work_function(void * params);
int isvalid(char* buffer, int bufferlen);
void ctob(char* string, int stringlen, BYTE *number);
int getval(BYTE character);
long long caltarget(unsigned int a, unsigned int b);


/* Writes out a connection log to the file.
 * Input: Client info struct.
 * Output: None.
 */
void connection_log(client_info_t);

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
