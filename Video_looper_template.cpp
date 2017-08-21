//============================================================================
// Name        : Escape_Rush_Entrance_Room_Video_Looper.cpp
// Author      : Joachim
// Version     :
// Copyright   : Gift to Escape Rush SPRL ;)
// Description :
//============================================================================
#include <mosquitto.h>
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
#define PORT 		1883
#define IP_BROKER 	"192.168.43.1"
#define TOPIC_CTRL 	"VideoControlTopic"


// Global variables


// Function prototypes
static void parseMessage(struct mosquitto * mosq, void * userdata, const struct mosquitto_message * message);
static void Delay(float delay);

// Globale variables
pid_t pid=0;
char sendBuff[256];
char recvBuff[256];
int sendBuffSize;
int recvBuffSize;


VideoControls video;


using namespace std;


int main(int argc, char *argv[])
{
	struct mosquitto * mosq = NULL;
	int messageID;
	mosquitto_lib_init();
	mosq = mosquitto_new(NULL,true,NULL);
	if(mosq == NULL)
	{
		perror("mosquitto_new:");
		return EXIT_FAILURE;
	}

	mosquitto_message_callback_set(mosq,parseMessage);

	if(mosquitto_connect(mosq,IP_BROKER,PORT,60) != 0)
	{
		perror("mosquitto connect:");
		return EXIT_FAILURE;
	}

	if(mosquitto_subscribe(mosq,&messageID,TOPIC_CTRL,2) != 0)
	{
		perror("mosquitto subscribe:");
		return EXIT_FAILURE;
	}


	// Start main loop
 	while(1)
 	{
		mosquitto_loop(mosq,300,1);
		Delay(0.1);
	}
    return EXIT_SUCCESS;
}

void parseMessage(struct mosquitto * mosq, void * userdata, const struct mosquitto_message * message)
{
	string s = (char*)message->payload;
	s.append(" ");
	size_t pos = 0;
	string cmd[5];
	uint8_t	i=0;

	fprintf(stderr,"%s\n",s.c_str());

	while( (pos=s.find(" ")) != string::npos)
	{
		cmd[i]=s.substr(0,pos);
		s.erase(0,pos+1);
		fprintf(stderr,"%s ",cmd[i].c_str());
		i++;
	}
	fprintf(stderr,"\n");

	if(!strcmp("start",cmd[0].c_str()))
		video.start(cmd[1].c_str(),atoi(cmd[2].c_str()));

	if(!strcmp("stop",cmd[0].c_str()))
		video.stop();

	if(!strcmp("reset",cmd[0].c_str()))
		video.reset();

	if(!strcmp("pause",cmd[0].c_str()))
		video.toggle();


}

void Delay(float delay)
{
	usleep((int)(delay*1000000));
}
