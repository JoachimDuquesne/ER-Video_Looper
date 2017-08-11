//============================================================================
// Name        : Escape_Rush_Entrance_Room_Video_Looper.cpp
// Author      : Joachim
// Version     :
// Copyright   : Gift to Escape Rush SPRL ;)
// Description :
//============================================================================

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <errno.h>

#include "VideoControls.h"

/*
Start omxplayer with the video and pause.
When videoLength is reached, reset the video.

Compile with : g++ Entrance-room-video-looper.cpp -lwiringPi -o Entrance-room-video-looper
Dependencies : 	- wiringPi
	 	- libpam-systemd

Note :	if the program is run from systemd during start,
	the active directory is NOT the one where the program is stored
	We need to use absolute path .
	Video file are provided by socket
*/


// Defines
#define PORT "55335"

// Global variables




// Function prototypes
static void parseRecvBuff(char recvBuff[], int recvBuffSize);
static void Delay(float delay);
static void sigchld_handler(int s);
static void *get_in_addr(struct sockaddr *sa);

// Globale variables
//char systemCallBuff[256];

int sockFD,newFD;
pid_t pid=0;
char sendBuff[256];
char recvBuff[256];
int sendBuffSize;
int recvBuffSize;


VideoControls video;


using namespace std;


int main(void)
{
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage clients_addr;
	socklen_t sin_size;
	struct sigaction sa;
	char s[INET_ADDRSTRLEN];
	int rv;
	int yes=1;

	memset(&hints,0,sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if( (rv=getaddrinfo(NULL,PORT,&hints,&servinfo)) != 0 )
	{
		fprintf(stderr,"getaddrinfo: %s\n",gai_strerror(rv));
		exit(EXIT_FAILURE);
	}

	for(p=servinfo; p!=NULL; p=p->ai_next)
	{
		if( (sockFD=socket(p->ai_family,p->ai_socktype,p->ai_protocol)) == -1)
		{
			perror("server: socket");
			continue; // Not a valid socket, continue down the list
		}
		fcntl(sockFD, F_SETFL, O_NONBLOCK);
		if( setsockopt(sockFD,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1 )
		{
			perror("setsockopt");
			exit(EXIT_FAILURE); // Unable to set socket options
		}
		if( bind(sockFD,p->ai_addr,p->ai_addrlen) == -1 )
		{
			close(sockFD);
			perror("server : bind");
			continue; // Unable to bind, try another addr down the list
		}
		break; // we found a correct socket, leave the for loop
	}
	freeaddrinfo(servinfo);
	if(p==NULL) // We reached then end of the list, so no valid socket found
	{
		fprintf(stderr,"server : failed to bind");
		exit(EXIT_FAILURE);
	}
	if(listen(sockFD,10) == -1)
	{
		perror("Listen");
		exit(EXIT_FAILURE);
	}
	sa.sa_handler = sigchld_handler; // reap all dead processes ??
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if(sigaction(SIGCHLD,&sa,NULL) == -1)
	{
		perror("sigaction");
		exit(EXIT_FAILURE);
	}
	fprintf(stdout,"server : waiting for connections ... \n");


	// Start main loop
//video.start("/home/EscapeRush/Video_Looper/tableau.mp4",20);
 	while(1)
 	{
		sin_size = sizeof clients_addr;
		newFD = accept(sockFD, (struct sockaddr *)&clients_addr,&sin_size);
		if(newFD == -1)
		{
		//	perror("accept");
			continue;
		}
		inet_ntop(clients_addr.ss_family,get_in_addr((struct sockaddr *)&clients_addr),s,sizeof s);
		fprintf(stdout,"server: got a connection from %s\n",s);

		if(!fork()) // This is the child process
		{
			close(sockFD); // Child don't need the parent socket FD
			sendBuffSize = sprintf((char*)sendBuff,"OmxServer: [cmd value]\n");
			if(send(newFD,sendBuff,sendBuffSize,0) == -1)
				perror("send");
			uint8_t j=255;
			while(1)
			{
				recvBuffSize = recv(newFD, recvBuff, 256-1, 0);
				Delay(0.5);

				if(recvBuffSize > 0) // If we received a new cmd
				{
					fprintf(stdout,"received:%s\n",recvBuff);
					parseRecvBuff(recvBuff,recvBuffSize);
				}

				if(j--)
				{
					j=255;
					fprintf(stdout,"\n");
				}
				fprintf(stdout,".");
			}

			close(newFD);
			exit(EXIT_SUCCESS);
		} // Child has exited

		close(newFD);

	}
    return 0;
}

void parseRecvBuff(char * recvBuff, int recvBuffSize)
{
	char cmd[3][256];
	uint8_t i=0,j=0,k=0;
	memset(&cmd,'\0',sizeof cmd);

	for(i=0 ; i<recvBuffSize && i<256 ; i++)
	{
		if( recvBuff[i] == ' ' || recvBuff[i] == '\n' )
		{
//			cmd[j][k]='\0';
			k=0;
			j++;
			continue;
		}
		cmd[j][k++]=recvBuff[i];
	}
//	cmd[j][k]='\0'; // the last argument was skipped if there was no space after it

	fprintf(stdout,"cmd:%s\narg1:%s\narg2:%s\n",&cmd[0][0],&cmd[1][0],&cmd[2][0]);
/*
	uint8_t cmdEnd,valueEnd;
	for(cmdEnd=0;cmdEnd<recvBuffSize;cmdEnd++)
		if(recvBuff[cmdEnd] == ' ' || recvBuff[cmdEnd] == '\n' || recvBuff[cmdEnd] == '\0')
			break; // cmdEnd-1 is the last char of the cmd

	recvBuff[cmdEnd] = '\0'; // Add a NULL char for "strcmp()"

	if(cmdEnd < recvBuffSize) // else there is no parameter
		for(valueEnd=cmdEnd+1;valueEnd<recvBuffSize;valueEnd++)
			if(recvBuff[valueEnd] == ' ' || recvBuff[valueEnd] == '\n' || recvBuff[valueEnd] == '\0')
				break; //  j-1 is the last char of the value

	for(uint8_t j=0;j+cmdEnd+1<valueEnd;j++)
		value[j] = recvBuff[cmdEnd+1+j];


	if(!strcmp("start",recvBuff))
	{
//		video.start();
	}

	if(!strcmp("stop",recvBuff))
		video.stop();

	if(!strcmp("reset",recvBuff))
		video.reset();

	if(!strcmp("pause",recvBuff))
		video.toggle();
*/
//printf(stdout,"cmdEnd=%d value=%s valueEnd=%d",cmdEnd,*value,valueEnd);
/*	switch(recvBuff[0])
	{
		case 'p': video.toggle();
			  break;
		case 'r': video.reset();
			  break;
		case 's': video.start("/home/EscapeRush/Video_Looper/tableau.mp4",20);
			  break;
		case 'q': video.stop();
			  break;
	}
*/
}

void Delay(float delay)
{
	usleep((int)(delay*1000000));
}


void sigchld_handler(int s)
{
	int saved_errno = errno;
	while(waitpid(-1,NULL,WNOHANG) > 0);
	errno = saved_errno;
}

void *get_in_addr(struct sockaddr *sa)
{
	return &(((struct sockaddr_in*)sa)->sin_addr);
}
