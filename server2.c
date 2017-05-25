/* A simple server in the internet domain using TCP
The port number is passed as an argument


 To compile: gcc server.c -o server
*/

#include "server.h"

int client_num = 0;
int work_num = 0;

extern struct work_job_t *head = NULL;
extern struct work_job_t *curr = NULL;


int main(int argc, char *argv[]) {

	int socket_fd, portno, clilen;
	char buffer[256];


	/* Ensure the port number was provided. */
	if(argc < N_ARGS) {
		fprintf(stderr, "Usage %s port_number.\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	/* Initialize mutex lock */
	if (pthread_mutex_init(&lock, NULL) != 0) {
        fprintf(stderr, "Mutex init failed.\n");
        exit(EXIT_FAILURE);
    }
	/* Client & server address */
	struct sockaddr_in client_addr, server_addr;

	/* Create a new log file each time server restarts */
	if((fp=fopen("log.txt", "w")) == NULL) {
		printf("Failed to create log file.\n");
		exit(EXIT_FAILURE);
	}
	fclose(fp);

	/* Create thread to do work */
	pthread_t work_thread_id;
	if(pthread_create(&work_thread_id, NULL, do_work, (void*)head)) {
		fprintf(stderr, "Failed to create thread.\n");
		exit(EXIT_FAILURE);
	}
	/* Detach this thread */
	if(pthread_detach(work_thread_id)) {
		fprintf(stderr, "Failed to detach work thead.\n");
		exit(EXIT_FAILURE);
	}else{
		fprintf(stderr, "work thread detached.\n");
	}

	/* Get a socket to listen on */
	//int socket_fd = initialize_server_socket(atoi(argv[1]), &server_addr);
	/* Create TCP socket */

 	socket_fd = socket(AF_INET, SOCK_STREAM, 0);

 	if (socket_fd < 0)
 	{
		//printf("ERROR opening socket\n");
 		perror("ERROR opening socket");
 		exit(1);
 	}

 	bzero((char *) &server_addr, sizeof(server_addr));

 	portno = atoi(argv[1]);

 	/* Create address we're going to listen on (given port number)
 	 - converted to network byte order & any IP address for
 	 this machine */

 	server_addr.sin_family = AF_INET;
 	server_addr.sin_addr.s_addr = INADDR_ANY;
 	server_addr.sin_port = htons(portno);  // store in machine-neutral format

 	 /* Bind address to the socket */

 	if (bind(socket_fd, (struct sockaddr *) &server_addr,
 			sizeof(server_addr)) < 0)
 	{
		//printf("ERROR on binding\n");
 		perror("ERROR on binding");
 		exit(1);
 	}

	int client_fd = 0;
  socklen_t client_len;

	/* Listen on this socket */
	if(listen(socket_fd, MAX_CLIENTS) < 0) {
		fprintf(stderr, "Failed to listen.\n");
		exit(EXIT_FAILURE);
	}

	/* Accept requests from clients */
	while(TRUE) {
		/* Get a new fd to communicate on */


		client_fd	 = accept(socket_fd, (struct sockaddr *)&client_addr,
			            &client_len);
    client_num++;
		printf("socket_fd is %d\n", socket_fd);
		printf("accept client_fd is %d\n", client_fd);
		if(client_fd < 0) {
			//printf("Error accepting client.\n");
			perror("Error accepting client.\n");
			exit(EXIT_FAILURE);
		}

    //close socket when reach max client num
    if(client_num >= MAX_CLIENTS){
      close(client_fd);
      printf("%d closed\n", client_fd);
      printf("number of client %d\n", client_num);
      client_num--;
      continue;
    }


		/* The server will communicate to the client on a new thread */
		/* Create a structure to pass in client information to the new
		 * thread.
		 */
		client_info_t *info;
		info = (client_info_t *)malloc(sizeof(*info));
		assert(info != NULL);
		info->client_fd = client_fd;
		info->client_addr = client_addr;
		info->server_addr = server_addr;
		info->newsocket_fd = socket_fd;
		/* Log this connection. */
		connection_log(*info);

		/* Create this new thread*/
		pthread_t thread_id;
		if(pthread_create(&thread_id, NULL, work_function, (void*)info)) {
			fprintf(stderr, "Failed to create thread.\n");
			exit(EXIT_FAILURE);
		}
		/* Detach this thread */
		if(pthread_detach(thread_id)) {
			fprintf(stderr, "Failed to detach thead.\n");
			exit(EXIT_FAILURE);
		}else{
			fprintf(stderr, "thread detached.\n");
		}
	}
	return 0;
}

void * work_function(void * params){
	client_info_t *client_info = (client_info_t *)params;

	 char buffer[256];
	 int n;
	 bzero(buffer,256);

	/* Read characters from the connection,
		then process */
while(1){
  bzero(buffer, 256);
	n = read(client_info->client_fd,buffer,255);

	if (n < 0) {
		perror("ERROR reading from socket");
		close(client_info->client_fd);
		pthread_exit(NULL);
	}
	if(buffer[0] == '\0'){
		close(client_info->client_fd);
		client_num--;
		pthread_exit(NULL);
	}
	if(strlen(buffer) != 0){
		receive_log(*client_info, buffer);
	}

	char firstFour[5];
	strncpy(&firstFour, buffer, 4);

	if(strcmp(buffer, "PING\r\n")==0){
		bzero(buffer, 256);
		strcpy(buffer, "PONG\r\n");
		n = write(client_info->client_fd,buffer,6);
		if (n < 0) {
			perror("ERROR reading from socket");
			close(client_info->client_fd);
			pthread_exit(NULL);
		}
		pong_log(*client_info);

	}else if(strcmp(buffer, "PONG\r\n")==0){
		bzero(buffer, 256);
		strcpy(buffer, "ERRO PONG message is strictly reserved for server.\r\n");
		n = write(client_info->client_fd,buffer,80);
		if (n < 0) {
			perror("ERROR reading from socket");
			close(client_info->client_fd);
			pthread_exit(NULL);;
		}
		error_log(*client_info, buffer);
	}else if(strcmp(buffer, "OKAY\r\n")==0){
		bzero(buffer, 256);
		strcpy(buffer, "ERRO It's not okay to send 'OKAY' messages to the server.\r\n");
		n = write(client_info->client_fd,buffer,80);
		if (n < 0) {
			perror("ERROR reading from socket");
			close(client_info->client_fd);
			pthread_exit(NULL);;
		}
		error_log(*client_info, buffer);
	}else if(strcmp(firstFour, "ERRO")==0){
		bzero(buffer, 256);
		strcpy(buffer, "ERRO This message should not be sent to the server.\r\n");
		n = write(client_info->client_fd,buffer,80);
		if (n < 0) {
			perror("ERROR reading from socket");
			close(client_info->client_fd);
			pthread_exit(NULL);;
		}
		error_log(*client_info, buffer);
	}else if((strcmp(firstFour, "SOLN")==0) && (strlen(buffer) == 97)){
		char difficulty[9];
		char seed[65];
		char solution[17];

		BYTE difficultyBYTE[32];
		BYTE seedBYTE[40];
		BYTE solutionBYTE[32];
		memcpy(&difficulty, buffer+5, 8);
		memcpy(&seed, buffer+14, 64);
		memcpy(&solution, buffer+79, 16);
		int flag = 0;
		int check = 0;
		for(check=0;check<8;check++){
			if(difficulty[check]=='\0'){
				flag = 1;
				break;
			}
		}
		if(flag==0){
			for(check=0;check<64;check++){
				if(seed[check]=='\0'){
					flag = 1;
					break;
				}
			}
		}
		if(flag==0){
			for(check=0;check<16;check++){
				if(solution[check]=='\0'){
					flag = 1;
					break;
				}
			}
		}
		if(flag){
			bzero(buffer, 256);
			strcpy(buffer, "ERRO \r\n");
			n = write(client_info->client_fd, buffer, strlen(buffer));
			if (n < 0) {
				perror("ERROR reading from socket");
				close(client_info->client_fd);
				pthread_exit(NULL);;
			}
		}else{
			ctob(difficulty, 8, difficultyBYTE);
			ctob(seed, 64, seedBYTE);
			ctob(solution, 16, solutionBYTE);
			BYTE target[32];
			getTarget(target, difficultyBYTE);
			int valid = isvalid(target, seedBYTE, solutionBYTE);
			bzero(buffer, 256);
			if(valid==1){
				strcpy(buffer, "OKAY\r\n");
				n = write(client_info->client_fd,buffer,6);
				if (n < 0) {
					perror("ERROR reading from socket");
					close(client_info->client_fd);
					pthread_exit(NULL);;
				}
				okay_log(*client_info);
			}else{
				strcpy(buffer, "ERRO not a valid solution.\r\n");
				n = write(client_info->client_fd,buffer,80);
				if (n < 0) {
					perror("ERROR reading from socket");
					close(client_info->client_fd);
					pthread_exit(NULL);;
				}
				error_log(*client_info, buffer);
			}
		}
	}else if(strcmp(firstFour, "WORK")==0){
    work_num++;
    printf("current work job %d\n", work_num);
    if(work_num >= MAX_WORK || strlen(buffer) != 100){
			bzero(buffer, 256);
			strcpy(buffer, "ERRO \r\n");
			n = write(client_info->client_fd, buffer, strlen(buffer));
			work_num--;
			if (n < 0) {
				perror("ERROR reading from socket");
				close(client_info->client_fd);
				pthread_exit(NULL);;
			}
		}else{

		char difficulty[9];
		char seed[65];
		char start[17];
		char counter[3];

		BYTE difficultyBYTE[32];
		BYTE seedBYTE[40];
		BYTE startBYTE[32];

		memcpy(&difficulty, buffer+5, 8);
		memcpy(&seed, buffer+14, 64);
		memcpy(&start, buffer+79, 16);
		memcpy(&counter, buffer+96, 2);

		int flag = 0;
		int check = 0;
		for(check=0;check<2;check++){
			if(counter[check]== '\0'){
				flag = 1;
				break;
			}
		}
		for(check=0;check<8;check++){
			if(difficulty[check]=='\0'){
				flag = 1;
				break;
			}
		}
		if(flag==0){
			for(check=0;check<64;check++){
				if(seed[check]=='\0'){
					flag = 1;
					break;
				}
			}
		}
		if(flag==0){
			for(check=0;check<16;check++){
				if(start[check]=='\0'){
					flag = 1;
					break;
				}
			}
		}
		if(flag){
			bzero(buffer, 256);
			strcpy(buffer, "ERRO \r\n");
			n = write(client_info->client_fd, buffer, strlen(buffer));
			if (n < 0) {
				perror("ERROR reading from socket");
				close(client_info->client_fd);
				work_num--;
				pthread_exit(NULL);;
			}
		}else{
			ctob(difficulty, 8, difficultyBYTE);
			ctob(seed, 64, seedBYTE);
			ctob(start, 16, startBYTE);
			//work(buffer, 256, difficultyBYTE, seedBYTE, startBYTE);
			add_to_queue(difficultyBYTE, seedBYTE, startBYTE, client_info);
			printf("aaaaaa\n");
	    //work_num--;
			//n = write(client_info->client_fd,buffer,strlen(buffer));
			//if (n < 0) {
			//	perror("ERROR reading from socket");
			//	close(client_info->client_fd);
			//	pthread_exit(NULL);;
			//}
			//solu_log(*client_info, buffer);
		}
	}
}else if(strcmp(firstFour, "ABRT")==0){
	strcpy(buffer, "OKAY\r\n");
	n = write(client_info->client_fd,buffer,6);
	if (n < 0) {
		perror("ERROR reading from socket");
		close(client_info->client_fd);
		pthread_exit(NULL);;
	}
	//TODO find work and delete
	okay_log(*client_info);
}else{
	strcpy(buffer, "ERRO\r\n");
	n = write(client_info->client_fd,buffer,80);
	if (n < 0) {
		perror("ERROR reading from socket");
		close(client_info->client_fd);
		pthread_exit(NULL);;
	}
	error_log(*client_info, buffer);
}
}
}

int isvalid(BYTE* target, BYTE* seedBYTE, BYTE *solutionBYTE){

	 BYTE x[40];

	int i = 0;
	for(i=0;i<8;i++){
		seedBYTE[i+32] = solutionBYTE[i+24];
	}

  //hash 1
	SHA256_CTX ctx1;
	BYTE buf[SHA256_BLOCK_SIZE];
	sha256_init(&ctx1);
	sha256_update(&ctx1, seedBYTE, 40);
	sha256_final(&ctx1, buf);

  //hash 2
	SHA256_CTX ctx2;
	BYTE buf2[SHA256_BLOCK_SIZE];
	sha256_init(&ctx2);
	sha256_update(&ctx2, buf, 32);
	sha256_final(&ctx2, buf2);


	return sha256_compare(target, buf2);
}

void getTarget(BYTE* target, BYTE* difficultyBYTE){
	uint32_t a;
	BYTE b[32];
	BYTE expResult[32];
	BYTE exp[32];
	BYTE two[32];


	uint256_init(b);
	uint256_init(exp);
	uint256_init(target);
	uint256_init(expResult);
	uint256_init(two);


	two[31] = 0x02;
	a = difficultyBYTE[28] - 3;
	a = a*8;

	int i = 0;
	for(i=0;i<3;i++){
		b[i+29] = difficultyBYTE[i+29];
	}


	uint256_exp(expResult, two, a);


	uint256_mul(target, b, expResult);

}

char * work(char *buffer, int bufferlen, BYTE *difficultyBYTE, BYTE *seedBYTE, BYTE *startBYTE){
	int valid = 0;
	BYTE nonce[32];
	BYTE one[32];
	BYTE target[32];

	uint256_init(one);
	uint256_init(nonce);
	getTarget(target, difficultyBYTE);

	int i = 0;
	for(i=0;i<32;i++){
		nonce[i] = startBYTE[i];
	}

	one[31] = 0x01;

	valid = isvalid(target, seedBYTE, nonce);

	while(valid != 1){
		uint256_add(nonce, nonce, one);

		valid = isvalid(target, seedBYTE, nonce);
	}
	bzero(buffer, 4);
	strncpy(buffer, "SOLN", 4);
	char solution[17];
	btoc(nonce+24, 16, solution);
	bzero(buffer+79, 20);
	strncpy(buffer+79, solution, 16);

}

void ctob(char* string, int stringlen, BYTE *number){
	int i = 0;
	int j = 0;
	uint256_init(number);
	for(i=stringlen-2;i>=0;i-=2){
		BYTE buffer[2];
		buffer[0] = getval(string[i]);
		buffer[1] = getval(string[i+1]);
		number[31-j] = buffer[0] * 16 + buffer[1];
		j++;
	}
}

void btoc(BYTE *number, int numberlen, char *string){
	char character[2];
	int a,b;
	int i = 0;
	int j = 0;
	for(i=0;i<numberlen;i++){
		a = number[i] / 16;
		b = number[i] % 16;
		character[0] = getcharacter(a);
		character[1] = getcharacter(b);
		string[j] = character[0];
		string[j+1] = character[1];
		j+=2;
	}

}

int getval(BYTE character){
	switch (character) {
		case '0': return 0;
		case '1': return 1;
		case '2': return 2;
		case '3': return 3;
		case '4': return 4;
		case '5': return 5;
		case '6': return 6;
		case '7': return 7;
		case '8': return 8;
		case '9': return 9;
		case 'a': return 10;
		case 'b': return 11;
		case 'c': return 12;
		case 'd': return 13;
		case 'e': return 14;
		case 'f': return 15;
	}
}

char getcharacter(int number){
	switch (number) {
		case 0: return '0';
		case 1: return '1';
		case 2: return '2';
		case 3: return '3';
		case 4: return '4';
		case 5: return '5';
		case 6: return '6';
		case 7: return '7';
		case 8: return '8';
		case 9: return '9';
		case 10: return 'a';
		case 11: return 'b';
		case 12: return 'c';
		case 13: return 'd';
		case 14: return 'e';
		case 15: return 'f';
	}
}

void connection_log(client_info_t client_info){
	FILE *fp=fopen("log.txt", "a");
	time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  fprintf(fp, "Timestamp: %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1,tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	char ip[20];
	inet_ntop(AF_INET, &(client_info.client_addr.sin_addr), ip, 20);
	fprintf(fp, "CONNECTION  client_addr %s   client_fd %d\n", ip, client_info.client_fd);
	fprintf(stderr, "CONNECTION  client_addr %s   client_fd %d\n", ip, client_info.client_fd);
	fclose(fp);
}

void okay_log(client_info_t client_info){
	FILE *fp=fopen("log.txt", "a");
	time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  fprintf(fp, "Timestamp: %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1,tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	char ip[20];
	inet_ntop(AF_INET, &(client_info.client_addr.sin_addr), ip, 20);
	fprintf(fp, "SEND TO  client_addr %s   client_fd %d  message OKAY\n", ip, client_info.client_fd);
	fprintf(stderr, "SEND TO  client_addr %s   client_fd %d  message OKAY\n", ip, client_info.client_fd);
	fclose(fp);
}
void pong_log(client_info_t client_info){
	FILE *fp=fopen("log.txt", "a");
	time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  fprintf(fp, "Timestamp: %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1,tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	char ip[20];
	inet_ntop(AF_INET, &(client_info.client_addr.sin_addr), ip, 20);
	fprintf(fp, "SEND TO  client_addr %s   client_fd %d  message PONG\n", ip, client_info.client_fd);
	fprintf(stderr, "SEND TO  client_addr %s   client_fd %d  message PONG\n", ip, client_info.client_fd);
	fclose(fp);
}
void error_log(client_info_t client_info, char *msg){
	FILE *fp=fopen("log.txt", "a");
	time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  fprintf(fp, "Timestamp: %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1,tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	char ip[20];
	inet_ntop(AF_INET, &(client_info.client_addr.sin_addr), ip, 20);
	fprintf(fp, "SEND TO  client_addr %s   client_fd %d  message %s\n", ip, client_info.client_fd, msg);
	fprintf(stderr, "SEND TO  client_addr %s   client_fd %d  message %s\n", ip, client_info.client_fd, msg);
	fclose(fp);
}
void solu_log(client_info_t client_info, char *msg){
	FILE *fp=fopen("log.txt", "a");
	if(fp == NULL){
		printf("failed to open log\n");
	}
	time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  fprintf(fp, "Timestamp: %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1,tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	char ip[20];
	inet_ntop(AF_INET, &(client_info.client_addr.sin_addr), ip, 20);
	fprintf(fp, "SEND TO  client_addr %s   client_fd %d  message %s\n", ip, client_info.client_fd, msg);
	fprintf(stderr, "SEND TO  client_addr %s   client_fd %d  message %s\n", ip, client_info.client_fd, msg);
	fclose(fp);
}
void receive_log(client_info_t client_info, char *msg) {
	FILE *fp = fopen("log.txt", "a");
	time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  fprintf(fp, "Timestamp: %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1,tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	char ip[20];
	inet_ntop(AF_INET, &(client_info.client_addr.sin_addr), ip, 20);
	fprintf(fp, "FROM client_addr %s   client_fd %d  message %s\n", ip, client_info.client_fd, msg);
	fprintf(stderr, "FROM client_addr %s   client_fd %d  message %s\n", ip, client_info.client_fd, msg);
	fclose(fp);
}
void disconnect_log(client_info_t client_info){
	FILE *fp=fopen("log.txt", "a");
	time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  fprintf(fp, "Timestamp: %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1,tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	char ip[20];
	inet_ntop(AF_INET, &(client_info.client_addr.sin_addr), ip, 20);
	fprintf(fp, "DISCONNECT  client_addr %s   client_fd %d\n", ip, client_info.client_fd);
	fprintf(stderr, "DISCONNECT  client_addr %s   client_fd %d\n", ip, client_info.client_fd);
	fclose(fp);
}

void add_to_queue(BYTE* difficulty, BYTE* seed, BYTE* start, client_info_t *info){
	head = add_to_list(seed, difficulty, start, info->client_fd, work_num, true, head, curr, info);
}
void* do_work(void * head){
	int n;
	char buffer[256];
	bzero(buffer, 256);
	head = (struct work_job_t*) head;
	while(1){
		if(head == NULL){
		  continue;
		}else{
			
			printf("have work in queue\n");
			curr = head;
			client_info_t *curr_client = curr->info;
			work(buffer, 256, curr->difficulty, curr->seed, curr->start);
			pop_head(head);
			work_num--;
			n = write(curr_client->client_fd,buffer,strlen(buffer));
			if (n < 0) {
				perror("ERROR reading from socket");
				close(curr_client->client_fd);
				continue;
			}
			solu_log(*curr_client, buffer);

		}
	}
}
