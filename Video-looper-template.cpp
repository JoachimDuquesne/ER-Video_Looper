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
unsigned int VideoLength = 60;
char const * VideoFile ="/home/EscapeRush/Video_Looper/tableau.mp4";
float const initialDelay = 1;		// number of seconds to play before pausing (when reseting video)


// Function prototypes
static void toggleVideo();
static void resetVideo();
static void startVideo(unsigned int index);
static void stopVideo();
static void Delay(float delay);
static void cleanup();
static void sigchld_handler(int s);
static void *get_in_addr(struct sockaddr *sa);

// Globale variables
//char systemCallBuff[256];
bool isPlaying ; // keep track if we are playing or if the video is in pause
time_t startTime=time(NULL),stopTime=time(NULL);
int pipeFD[2];
int sockFD,newFD;
pid_t pid=0;
char sendBuff[256];
char recvBuff[256];
uint8_t sendBuffSize;
uint8_t recvBuffSize;

using namespace std;

int main(void)
{
	//atexit(cleanup);
	uint8_t i=0;
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


//	startVideo(i);
//	Delay(1);
//	toggleVideo();
	
	// Start main loop

 	while(1)
 	{
		sin_size = sizeof clients_addr;
		newFD = accept(sockFD, (struct sockaddr *)&clients_addr,&sin_size);
		if(newFD == -1)
		{
			perror("accept");
			continue;
		}
		inet_ntop(clients_addr.ss_family,get_in_addr((struct sockaddr *)&clients_addr),s,sizeof s);
		fprintf(stdout,"server: got a connection from %s\n",s);




		if(!fork()) // This is the child process
		{
			startVideo(i);
			while(1)
			{
				close(sockFD); // Child don't need the parent socket FD
				sendBuffSize = sprintf((char*)sendBuff,"OmxServer:\"[cmd;value;]\"\n");
				if(send(newFD,sendBuff,sendBuffSize,0) == -1)
					perror("send");
				
				if ((recvBuffSize = recv(newFD, recvBuff, 256-1, 0)) == -1) 
				{
					perror("recv");
					exit(EXIT_FAILURE);
				}else
				{
					if(recvBuff[0]=='p')
						toggleVideo();
					if(recvBuff[0]=='r')
						resetVideo();
					if(recvBuff[0]=='q')
					{
						stopVideo();
						break;
					}
				}
			}
			
			close(newFD);
			exit(EXIT_SUCCESS);
		} // Child has exited
		
		
		
		close(newFD);
		// We will not reach the end of the video while in pause but the system time continue to roll, so ignore...
		if (isPlaying)
			if(time(NULL) >= startTime + VideoLength)
			{
				fprintf(stdout,"Restarting the video because videoLength has expired\n");
				stopVideo();
			}
	}
    return 0;
}


// Toggle between play and paused
// Update isPlaying, startTime and stopTime
void toggleVideo()
{
	if(pid <= 0) // We dont have a child running omxplayer so we can't toggle
	{
		fprintf(stdout,"No video playing, can't toggle\n");
		return;
	}

	write(pipeFD[1],"p",1);
	fsync(pipeFD[1]);

	if(isPlaying)
		stopTime = time(NULL); // if video was playing and is being paused, note the stopTime
	else // We compute a new startTime disregarding the pause
		startTime = time(NULL) - (stopTime-startTime);


	isPlaying = !isPlaying;

	if(isPlaying)
		fprintf(stdout,"play\n");
	else
	 	fprintf(stdout,"pause\n");

}



// Restart the video
void resetVideo()
{
	if(pid <= 0)
	{
		fprintf(stdout,"No video playing, can't reset\n");
		return;
	}

	fprintf(stdout,"Reseting the video \n");

	write(pipeFD[1],"i",1); // return to the previous chapter
	fsync(pipeFD[1]);

	startTime = time(NULL); // reseting the chrono
	stopTime  = time(NULL);


	// If the video was paused before reseting, launch it
	if(!isPlaying)
	    toggleVideo();
}


// Start omxplayer
void startVideo(unsigned int index)
{

	isPlaying = true;

	if(pid >0) // A child already exist
		stopVideo();

	pipe(pipeFD);	// Create a pipe between father and child process

	pid = fork();	// Spawn child process

	if(pid == -1)
	{// Check if spawn
		perror("Error forking omxplayer");
		exit(EXIT_FAILURE);
	}


	if(pid == 0)
	{ // We are in the child process : start omxxplayer
		close(pipeFD[1]);	// closing write side of the pipe
		dup2(pipeFD[0],STDIN_FILENO);
		char const * arguments[] = {"omxplayer","--no-osd","-o","hdmi",VideoFile,NULL};
		if( execv("/usr/bin/omxplayer",const_cast<char**>(arguments)) == -1)
		{
			perror("execv omxplayer");
			exit(EXIT_FAILURE);
		}
		// If the child reach here, that means we have send 'quit' to omxplayer to we kill the child process
		exit(EXIT_SUCCESS);
	}
	// Only the father process can arrive here
	startTime = time(NULL);
	stopTime  = time(NULL);
	close(pipeFD[0]); // closing the reading side of the pipe
}


void stopVideo()
{
	write(pipeFD[1],"q",1);
	fsync(pipeFD[1]);

	pid=0; // reset pid
	isPlaying = false;
	close(pipeFD[1]);
}


void Delay(float delay)
{
	usleep((int)(delay*1000000));
}


// Clean file and omxplayer when program quit
void cleanup()
{
	char const * arguments[] = {"killall","omxplayer.bin",NULL};
	if( execv("killall",const_cast<char**>(arguments)) == -1)
		perror("error killing omxplayer.bin");

	arguments[1] = "omxplayer";
	if( execv("killall",const_cast<char**>(arguments)) == -1)
		perror("error killing omxplayer");

	close(pipeFD[0]);
	close(pipeFD[1]);
	close(sockFD);
	close(newFD);
	fprintf(stdout,"Cleaning completed\n");
	exit(EXIT_SUCCESS);
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
