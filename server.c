/* A simple server in the internet domain using TCP
The port number is passed as an argument


 To compile: gcc server.c -o server
*/

#include "server.h"


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
	if((fp=fopen(OUT_FILE, "w")) == NULL) {
		printf("Failed to create log file.\n");
		exit(EXIT_FAILURE);
	}
	fclose(fp);

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
	//printf("Listening on port %d....\n", atoi(argv[1]));

	/* Accept requests from clients */
	while(TRUE) {
		/* Get a new fd to communicate on */
		//printf("socket_fd is %d\n", socket_fd);

		client_fd	 = accept(socket_fd, (struct sockaddr *)&client_addr,
			            &client_len);
		//printf("client_fd is %d\n", client_fd);
		if(client_fd < 0) {
			printf("Error accepting client.\n");
			//perror("Error accepting client.\n");
			exit(EXIT_FAILURE);
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
		//connection_log(*info);

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
	//printf("%d\n", client_info->client_fd);
	n = read(client_info->client_fd,buffer,255);

	if (n < 0) {
		perror("ERROR reading from socket");
		exit(1);
	}

	printf("%s\n",buffer);

	//TODO check message and reply
	char firstFour[5];
	strncpy(&firstFour, buffer, 4);
	printf("%s\n", firstFour);

	if(strcmp(buffer, "PING\n")==0){
		bzero(buffer, 256);
		strcpy(buffer, "PONG");
	}else if(strcmp(buffer, "OKAY\n")==0){
		bzero(buffer, 256);
		strcpy(buffer, "It's not okay to send 'OKAY' messages to the server.");
	}else if(strcmp(firstFour, "ERRO")==0){
		bzero(buffer, 256);
		strcpy(buffer, "This message should not be sent to the server.");
	}else if(strcmp(firstFour, "SOLN")==0){
		//TODO check isvalid(buffer) return 0 if valid

		isvalid(buffer, 255);
	}
	n = write(client_info->client_fd,buffer,255);

	if (n < 0) {
		perror("ERROR writing to socket");
		exit(1);
	}
}
int isvalid(char* buffer, int bufferlen){
	char difficulty[9];
	char seed[65];
	char solution[17];

	BYTE difficultyBYTE[DIFFICULTY_LEN];
	BYTE seedBYTE[SEED_LEN];
	BYTE solutionBYTE[SOLUTION_LEN];

	int difficultyINT;
	long long seedINT;
	long int solutionINT;
	char * ptr1, *ptr2, *ptr3;

	memcpy(&difficulty, buffer+5, 8);
	//printf("difficulty is %s\n", difficulty);
	memcpy(&seed, buffer+14, 64);
	//printf("seed is %s\n", seed);
	memcpy(&solution, buffer+79, 16);
	//printf("solution is %s\n", solution);

	difficultyINT = strtol(difficulty, &ptr1, 16);
	printf("difficulty is %x\n", difficultyINT);

	seedINT = strtol(seed, &ptr2, 16);
	printf("seed is %llx\n", seedINT);

	solutionINT = strtol(solution, &ptr3, 16);
	printf("solution is %lx\n", solutionINT);

	ctob(difficulty, 8, difficultyBYTE);
	ctob(seed, 64, seedBYTE);
	ctob(solution, 16, solutionBYTE);



	//get target
	// long long target;
	// unsigned int a, b;
	// a = difficultyBYTE[0];
	// b = difficultyBYTE[1]*16*16 + difficultyBYTE[2]*16 + difficultyBYTE[3];
	// target = caltarget(a, b);

	unsigned int a;
	BYTE b[32];
	BYTE expResult[32];
	BYTE exp[32];
	BYTE target[32];
	BYTE two[32];


	uint256_init(b);
	uint256_init(exp);
	uint256_init(target);
	uint256_init(expResult);
	uint256_init(two);



	two[31] = 0x02;

	a = difficultyBYTE[0] - 3;
	a = a*8;

	int i = 0;
	for(i=0;i<3;i++){
		b[i+28] = difficultyBYTE[i+1];
	}

	uint256_exp(expResult, two, a);
	print_uint256(expResult);

	uint256_mul(target, b, expResult);
	print_uint256(target);

	return 0;
}

void ctob(char* string, int stringlen, BYTE *number){
	int i = 0;
	int j = 0;
	for(i=0;i<stringlen;i+=2){
		BYTE buffer[2];
		buffer[0] = getval(string[i]);
		buffer[1] = getval(string[i+1]);
		number[j] = buffer[0] * 16 + buffer[1];
		j++;
	}
}

long long caltarget(unsigned int a, unsigned int b){
	long long target;
	unsigned int exp = 8 * (a - 3);
	long long power = (((long long)1) << exp);
	target = 8 * power;
	return target;
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
