#ifndef _VIDEO_CONTROLS_H_
#define _VIDEO_CONTROLS_H_


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

#define INVALID	0
#define START	1
#define STOP	2
#define RESET	3
#define PAUSE	4
#define QUIT	5
#define NEWFILE	6


class VideoControls {
	public:
		VideoControls();
		
		void toggleVideo();
		void resetVideo();
		void startVideo(char const * VideoFile, unsigned int Video_Length);
		bool getIsPlaying();
		bool getIsFinished();
		bool process(uint8_t cmd, char value[]);
		void stopVideo();
		float initialDelay;		// number of seconds to play before pausing (when reseting video)
		
	private:
		bool isPlaying ; // keep track if we are playing or if the video is in pause
		time_t startTime,stopTime;
		int pipeFD[2];
		pid_t pid;
		int VideoLength;
		char VideoFile[256];
		char command[256];

};
#endif /* _VIDEO_CONTROLS_H_ */
