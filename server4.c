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
	if((fp=fopen("log.txt", "w")) == NULL) {
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
