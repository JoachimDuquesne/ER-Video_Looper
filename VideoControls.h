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
#include <pthread.h>



class VideoControls {
	public:
		VideoControls();
		static void toggle();
		static void reset();
		static void start(char const * videofile, int videolength);
		static bool getIsPlaying();
		static bool getIsFinished();
		static void stop();

	private:
		static volatile bool isPlaying; // keep track if we are playing or if the video is in pause
		static volatile bool isFinished;
		static time_t startTime,stopTime;
		static int pipeFD[2];
		static pthread_t thread;
		static volatile pid_t pid;
		static int childStatus;
		static int videoLength;
		static void * monitoring(void * arg);

};
#endif /* _VIDEO_CONTROLS_H_ */
