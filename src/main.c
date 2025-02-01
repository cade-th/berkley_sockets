#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h> // memcpy, memset
#include <unistd.h> // read, write, close system calls
#include <arpa/inet.h> // socket-related calls
#include <netdb.h> // more socket related structures

#define SERV_PORT 3000 // Server port number
#define LISTEN_BACKLOG 5 // number of connections to keep waiting
#define BUFSIZE 1024

// TODO: find some macro magic to automate the types of these fields so they're not all strings maybe?
#define CONFIG_FIELDS \
    X(char *, program)  \
    X(char *, type)      \
    X(char *, hostname)  \
    X(char *, port)  

typedef struct {
    #define X(type, name) type name;
    CONFIG_FIELDS
    #undef X
} Config;

Config parse_config(char *argv[]) {
    Config config;
    int i = 0;

    #define X(type, name) config.name = argv[i++];
    CONFIG_FIELDS
    #undef X

    return config;
}

void server(char *program, char *hostname, int port) {
	int list_sock, comm_sock;
	struct sockaddr_in server; // Server address
	
	char buf[BUFSIZE]; // buffer for reading data from socket
	int bytes_read; // # bytes read from socket
	
	// 1. Create socket on which to listen (file descriptor)
	list_sock = socket(AF_INET, SOCK_STREAM, 0);

	if (list_sock < 0) {
		perror("opening socket stream");
		exit(1);
	}

	// 2. Initiate the fields of the server address
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port);

	// 3. Bind the socket to the given server address
	if (bind(list_sock, (struct sockaddr *) &server, sizeof(server))) {
		perror("binding socket name");
		exit(2);
	}

	// 4. Start accpeting connections, and allow one connection at a time
	listen(list_sock, LISTEN_BACKLOG);
	while(true) {
		// Accept conneciton and receive communication socket (file descriptor)
		comm_sock = accept(list_sock,0,0);
		if (comm_sock == -1)
			perror("accpt");
		else {
			printf("Connection started\n");
			do {
				memset(buf, 0, sizeof(buf)); // fill buffer with nulls
				if ((bytes_read = read(comm_sock, buf,	BUFSIZE-1)) < 0)
					perror("reading stream message");
				if (bytes_read == 0) 
					printf("Ending connection\n");
				else 
					printf("\t%s", buf);	
			} while (bytes_read != 0); 

				close(comm_sock);

				printf("Connection ended\n");
			}
		}

		close(list_sock);

		
}

void client(char *program, char *hostname, int port) {
	
	// 1. Create socket
	int comm_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (comm_sock < 0) {
		perror("opening socket");
		exit(2);
	}

	// 2. Initialize the fields of the server address
	struct sockaddr_in server; // address of the server
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	// Look up the hostname specified on command line
	struct hostent *hostp = gethostbyname(hostname); // server hostname
	if (hostp == NULL) {
		fprintf(stderr, "%s: unknown host '%s'\n", program, hostname);
		exit(3);
	}
	memcpy(&server.sin_addr, hostp->h_addr_list[0], hostp->h_length);

	// 3. Connect the socket to that server
	if (connect(comm_sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
		perror("connecting stream socket");
		exit(4);
	}
	printf("Connected!\n");

	// 4. Read content from stdin (file descriptor 0) and write to socket)
	char buf[BUFSIZE]; // a buffer for reading data from stdin
	int bytes_read; // # bytes read from socket
	memset(buf, 0, BUFSIZE); // clear up the buffer
	do {
		if ((bytes_read = read(0,buf, BUFSIZE - 1)) < 0) {
			
		} else {
			if (write(comm_sock, buf, bytes_read) < 0) {
				perror("writing on stream socket");
				exit(6);
			}
		}
	} while (bytes_read > 0);

	close(comm_sock);
}

void run(Config *config) {
	strcmp(config->type, "server") == 0 ? 
	  server(config->program, config->hostname, atoi(config->port)) 
	: client(config->program, config->hostname, atoi(config->port));
}		

int main(const int argc, char *argv[]) {
	if (argc != 4) {
		printf("Usage: %s <type> <hostname> <port>\n", argv[0]);
		return 1;
	}

	Config config = parse_config(argv);

	run(&config);

	return 0;
}


