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
	printf("Listening on port %d....\n", atoi(argv[1]));
	printf("aaaaa\n");
	/* Accept requests from clients */
	while(TRUE) {
		/* Get a new fd to communicate on */
		//printf("socket_fd is %d\n", socket_fd);

		client_fd	 = accept(socket_fd, (struct sockaddr *)&client_addr,
			            &client_len);
		printf("client_fd is %d\n", client_fd);
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
	printf("%d\n", client_info->client_fd);
	n = read(client_info->client_fd,buffer,255);

	if (n < 0)
	{
		perror("ERROR reading from socket");
		exit(1);
	}

	printf("%s\n",buffer);

	//TODO check message and reply
	if(strcmp(buffer, "PING\n")==0){
		bzero(buffer, 256);
		strcpy(buffer, "PONG");
	}else if(strcmp(buffer, "OKAY\n")==0){
		bzero(buffer, 256);
		strcpy(buffer, "It's not okay to send 'OKAY' messages to the server.");
	}else if(strcmp()
	n = write(client_info->client_fd,buffer,255);

	if (n < 0)
	{
		perror("ERROR writing to socket");
		exit(1);
	}

}
