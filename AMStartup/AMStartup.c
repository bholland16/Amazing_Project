/*
 ============================================================================
 Name        : AMStartup.c
 Authors     : Benjamin Page, Benjamin Holland, Xinran Xiao
 Date 	     : May 27th, 2013
 Description : Startup program for Amazing Project for cs50

 Input	     : Number of avatars, difficulty, hostname
 Output	     : Error messages, logfile, various messages to user, avatars
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include <time.h>
#include "AMStartup.h"
#include "amazing.h"

/*=============================================================================*/
/*				   MAIN  			               */
/*=============================================================================*/

int main(int argc, char** argv)
{

	unsigned int mazePort, width, height, difficulty, nAvatars;
	int sockfd;
	char *ip, *logName;
	struct sockaddr_in servaddr;
	struct hostent *host;
	ssize_t recvrc;

	// Command line processing on arguments
	if (argc != 4)
	{
		fprintf(stderr, "You entered %d arguments\n", argc - 1);
		fprintf(stderr,
				"AMStartup Usage: ./AMStartup [NUMBER OF AVATARS] [DIFFICULTY] [HOSTNAME]\n");
		return EXIT_FAILURE;
	}

	// Verify difficulty
	if (!isValidLong(argv[2]))
	{
		fprintf(stderr, "Invalid difficulty. Enter a valid integer.\n");
		return EXIT_FAILURE;
	}

	difficulty = (unsigned int) strtol(argv[2], NULL, 10);
	if (difficulty < 0 || difficulty > 9)
	{
		fprintf(stderr, "Invalid difficulty, must be between 0-9 inclusive.\n");
		return EXIT_FAILURE;
	}

	// Verify number of avatars
	if (!isValidLong(argv[1]))
	{
		fprintf(stderr, "Invalid number of avatars.\n");
		return EXIT_FAILURE;
	}
	nAvatars = (unsigned int) strtol(argv[1], NULL, 10);
	if (nAvatars > AM_MAX_AVATAR || nAvatars < 2)
	{
		fprintf(stderr, "Invalid number of avatars (maximum is %d).\n", AM_MAX_AVATAR);
	}

	// Get IP from host name
	host = gethostbyname(argv[3]);
	if (!host)
	{
		fprintf(stderr, "Invalid hostname: %s\n", argv[3]);
		return EXIT_FAILURE;
	}
	ip = inet_ntoa(*((struct in_addr *) host->h_addr_list[0]));	// inet_ntoa converts it into X.X.X.X
	printf("stratton IP: %s\n", ip);

	// Create a socket descriptor for the client
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		// <0 means there was an error in creation of the socket
		fprintf(stderr, "Error creating socket to connect to server.\n");
		return EXIT_FAILURE;
	}
	printf("sockfd: %d\n", sockfd);

	// Create a socket address structure
	BZERO(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(ip);
	servaddr.sin_port = htons(AM_SERVER_PORT);

	// Connect to the socket
	if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
	{
		fprintf(stderr, "Problem connecting to the server.\n");
		return EXIT_FAILURE;
	}

	// Build initialize message
	AM_MESSAGE message;
	message.message_type = htonl(AM_INITIALIZE);
	message.msg.initialize.Difficulty = htonl(difficulty);
	message.msg.initialize.nAvatars = htonl(nAvatars);

	printf("difficulty: %d, number: %d\n", difficulty, nAvatars);

	// Send initialize message
	if (send(sockfd, &message, sizeof(message), 0) < 0)
	{
		fprintf(stderr, "Error sending initialization message.\n");
		return EXIT_FAILURE;
	}

	// Listen to the server for a returned message
	AM_MESSAGE receivedMessage;
	if ((recvrc = recv(sockfd, &receivedMessage, sizeof(receivedMessage), 0)) == 0)
	{
		fprintf(stderr, "The server terminated prematurely.\n");
		return EXIT_FAILURE;
	}

	// Interpret the message received
	struct passwd *p;
	uid_t uid;
	pid_t pids[nAvatars];
	char widthC[MAX_CHAR_LIMIT], heightC[MAX_CHAR_LIMIT], nAvatarsC[MAX_CHAR_LIMIT], difficultyC[MAX_CHAR_LIMIT],
			mazePortC[MAX_CHAR_LIMIT], iC[MAX_CHAR_LIMIT];

	switch (ntohl(receivedMessage.message_type))
	{
	case AM_INITIALIZE_OK:
		// Get fields
		mazePort = ntohl(receivedMessage.msg.initialize_ok.MazePort);
		width = ntohl(receivedMessage.msg.initialize_ok.MazeWidth);
		height = ntohl(receivedMessage.msg.initialize_ok.MazeHeight);

		// Get $USER
		if ((p = getpwuid(uid = getuid())) == NULL )
		{
			fprintf(stderr, "Unable to find UID to create log file.\n");
			return EXIT_FAILURE;
		}

		// Create logName
		logName = malloc(
				snprintf(NULL, 0, "Amazing_%s_%d_%d.log", p->pw_name, nAvatars, difficulty) + 1);
		sprintf(logName, "Amazing_%s_%d_%d.log", p->pw_name, nAvatars, difficulty);
		printf("%s\n", logName);	// debug

		// Create log and write first line
		char buffer[MAX_CHAR_LIMIT];
		sprintf(buffer, "echo %s, %d, `date` > %s", p->pw_name, mazePort, logName);
		system(buffer);

		// Create avatar_client's
		int i;
		printf("mazePort: %d, width: %d, height: %d\n", mazePort, width, height);

		sprintf(nAvatarsC, "%d", nAvatars);
		sprintf(difficultyC, "%d", difficulty);
		sprintf(mazePortC, "%d", mazePort);
		sprintf(widthC, "%d", width);
		sprintf(heightC, "%d", height);

		// Forks a different process for each avatar it needs to create.
		// The parent will exit while the children will execl() the avatar_clients
		for (i = 0; i < nAvatars; i++)
		{
			pids[i] = fork();

			if (pids[i] < 0)
			{
				fprintf(stderr, "Fork unsuccessful.\n");
				free(logName);
				return EXIT_FAILURE;
			}
			else if (pids[i] == 0)
			{
				printf("child");
				sleep(5);

				// Executed in child
				sprintf(iC, "%d", i);

				if (execl("../avatar_client/avatar_client", "avatar_client", iC, nAvatarsC, difficultyC, ip, mazePortC, logName, widthC, heightC, (char *) NULL ) == -1)
				{
					fprintf(stderr, "execl error.\n");
					free(logName);
					return EXIT_FAILURE;
				}
			}
			else
			{
				// Executed in parent
				printf("parent");
			}
		}

		free(logName);

		break;

	case AM_INITIALIZE_FAILED:
		fprintf(stderr, "Received an AM_INITIALZE_FAIL message.\n");
		return EXIT_FAILURE;

	case AM_UNKNOWN_MSG_TYPE:
		fprintf(stderr, "Received an AM_UNKNOWN_MSG_TYPE message.\n");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*Function to determine if something is a valid long integer*/
int isValidLong(char *x)
{
	char* endp;

	strtol(x, &endp, 10);

	return !*endp;
}
