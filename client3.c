/* Brittany Stewart
CS 535
Assignment 3 
Client 3 */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>

main(int argc, char *argv[]) {
	
	char buffer[32]; /* buffer for messages sent between client and server */
	int csock; /* client socket to connect to server */
	int cmp, conn, w, r, c;
	int port = atoi(argv[2]); /* get port number from command line and convert to integer */
	fd_set rfds; /* file descriptor set */
	int bytes, i;

	struct sockaddr_in RA; /* structure to handle server address */
	struct hostent *host; /* structure to handle server host name */
	csock = socket(AF_INET, SOCK_STREAM, 0); /* define client socket and check for errors */
	if (csock == -1) {
		perror("socket");
	} 
	
	memset(&RA, 0, sizeof(RA)); /* initialize server address structure */
	
	RA.sin_family = AF_INET; 
	host = gethostbyname(argv[1]); /* get server information by given host name */
	if (host == NULL) { /* check for errors */
		perror("gethostbyname");
	}

	/* copy host address to server address */
	memcpy((unsigned char *)&RA.sin_addr, (unsigned char *)host->h_addr, host->h_length);
	
	RA.sin_port = htons(port); /* set port number and convert to network short*/
	
	conn = connect(csock, (struct sockaddr *)&RA, sizeof(RA)); /* connect client socket to server */
	if (conn == -1) { /* check for errors */
		perror("connect");
	}
	while(1) { /* connection stays open while client continually looks for input from keyboard and data from server */
		FD_ZERO(&rfds); /* clear the file descriptor set */
		FD_SET(0, &rfds); /* add stdin to the file descriptor set */
		FD_SET(csock, &rfds); /* add the client socket to the file descriptor set */

		i = select(16, &rfds, 0, 0, 0); /* call select */
		
		if (FD_ISSET(0, &rfds)) { /* if stdin is set in file descriptor set */
			bytes = read(fileno(stdin), buffer, 512); /* read from keyboard */
			buffer[bytes] = 0; /* add end of line character */
			if(bytes <= 0) break;
			w = write(csock, buffer, bytes + 1); /* write string to server */
			if (w == -1) {
				perror("write\n");
			}
		}

		if (FD_ISSET(csock, &rfds)) { /* if client socket is set in file descriptor */
			bytes = recv(csock, buffer, 512, 0); /* receive data from server */
			if(bytes <= 0) break;
			printf("CLIENT: message from server: %s \n", buffer);
			if (strcmp(buffer, "end\n") == 0) { /* if string is end, connection is closed and client exits */
				c = close(csock); /* close socket */
				if (c == -1) { /* check for errors */
					perror("close");
				}
				printf("CLIENT: exit \n"); /* client exits */
				exit(0);
			}
			if (strcmp(buffer, "quit\n") == 0) { /* if string is quit, connection is closed and client exits */
				c = close(csock); /* close socket */
				if (c == -1) { /* check for errors */
					perror("close");
				}
				printf("CLIENT: exit \n"); /* client exits */
				exit(0);
			}
		}		
	}
}