/* A simple server in the internet domain using TCP
The port number is passed as an argument


 To compile: gcc server.c -o server
*/

#include "server.h"


int main(int argc, char *argv[]) {
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
	int socket_fd = initialize_server_socket(atoi(argv[1]), &server_addr);
	int client_fd = 0;
  	socklen_t client_len;

	/* Listen on this socket */
	if(listen(socket_fd, MAX_CLIENTS) < 0) {
		fprintf(stderr, "Failed to listen.\n");
		exit(EXIT_FAILURE);
	}
	printf("Listening on port %d....\n", atoi(argv[1]));

	/* Accept requests from clients */
	while(TRUE) {
		/* Get a new fd to communicate on */
		client_fd = accept(socket_fd, (struct sockaddr *)&client_addr,
			            &client_len);
		if(client_fd < 0) {
			perror("Error accepting client.\n");
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
		}
	}
	return 0;
}

void * work_function(void * params){
	client_info_t *client_info = (client_info_t *)params;

	bzero(buffer,256);

	/* Read characters from the connection,
		then process */

	n = read(newsockfd,buffer,255);

	if (n < 0)
	{
		perror("ERROR reading from socket");
		exit(1);
	}

	printf("Here is the message: %s\n",buffer);

	n = write(newsockfd,"I got your message",18);

	if (n < 0)
	{
		perror("ERROR writing to socket");
		exit(1);
	}

}
