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


class VideoControls {
	public:
		VideoControls();
		
		float initialDelay;		// number of seconds to play before pausing (when reseting video)
		
		void toggleVideo();
		void resetVideo();
		void startVideo(char const * VideoFile, unsigned int Video_Length);
		void stopVideo();

	private:
		bool isPlaying ; // keep track if we are playing or if the video is in pause
		time_t startTime,stopTime;
		int pipeFD[2];
		pid_t pid;
		unsigned int VideoLength;

};
#endif /* _VIDEO_CONTROLS_H_ */
