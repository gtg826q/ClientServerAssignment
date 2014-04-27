/* Brittany Stewart
CS 535
Assignment 3 
Server 3 */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/select.h>

void SignalInterrupt (int sigtype) { /* signal handler */
	signal(SIGCHLD, SignalInterrupt);
	printf("caught a signal <%d>\n", sigtype); /* print signal type */
	while (1) {
		int status, r, s;
		r = waitpid(-1, &status, WNOHANG); /* obtain exit status information about a child process */
		if (r <= 0) return; /* return if there is an error or if status is not available for child process */
		s = WEXITSTATUS(status); /* set s to status of child process */
		printf("exit status = %d \n", s);
		if (s == 1) { /* if s is 1 server exits */
			kill(0, 9);
			exit(0);
		}
	}
}

void main(int argc, char *argv[]) {
	struct sockaddr_in SAP;
	int ssock, stc;
	/*int procid, tmp;*/
	int b, l, c, w, n, p;
	int port = atoi(argv[1]); /* get port number from command line and convert to integer */
	int i;
	
	signal(SIGCHLD, SignalInterrupt);
	char buffer[512]; /* buffer for messages sent between client and server */
	int msg; /* integer to store the result of reading message from client */
	ssock = socket(AF_INET, SOCK_STREAM, 0); /* define server socket and check for errors */
	if (ssock == -1) {
		perror("socket");
	}
	SAP.sin_family = AF_INET;
	SAP.sin_addr.s_addr = htonl(INADDR_ANY);
	SAP.sin_port = htons(port); /* set port number and convert to network short */
	b = bind(ssock, (struct sockaddr *) &SAP, sizeof(struct sockaddr)); /* assign address to server socket */
	if (b == -1) { /* check for errors */
		perror("bind");
	}
	l = listen(ssock, 5); /* specify willingness to accept incoming connections and queue limit */
	if (l == -1) { /* check for errors */
	perror("listen");

	}
	while(1) { /* keep listening for connections */
		stc = accept(ssock, 0, 0); /* accept connection from client */
			if (stc == -1) { /* check for errors */
				perror("accept");
			}
		
		i = fork(); /* create new child process */
		if ( i == 0) { /* if child process */
			c = close(ssock);
			if (c == -1) {
				perror("close");
			}
			while(1) {
				msg = read(stc, buffer, 512); /* read message from client off buffer */
				if (msg == -1) { /* check for errors */
					perror("read");
				}
				if (msg == 0) {
					printf("Error: client disconnected.\n");
					exit(0);
				}

				printf("SERVER: msg from client: %s \n", buffer); 
				w = write(stc, buffer, 5); /* write message back to client */
				if (w == -1) { /* check for errors */
					perror("write");
				}
				if (strcmp(buffer, "quit\n") == 0) { /* if string from client is quit, server exits */
					c = close(stc);
					if (c == -1) {
						perror("close");
					}	
					printf("SERVER: quit \n");
					exit(1);
				}
			
				if (strcmp(buffer, "end\n") == 0) { /* if string client is end, client exits */
					c = close(stc);
					if (c == -1) {
						perror("close");
					}
					exit(0);
				}
			}
		}
	}
}