#ifndef _VIDEO_CONTROLS_H_
#define _VIDEO_CONTROLS_H_


#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
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
#include <thread>



class VideoControls {
	public:
		VideoControls();
		static void Init();
		static void Toggle();
		static void Pause();
		static void Play();
		static void Reset();
		static void Start(char const * videofile, int videolength);
		static bool GetIsPlaying();
		static bool GetIsFinished();
		static void Stop();

	private:
		static volatile bool isPlaying; // keep track if we are playing or if the video is in pause
		static volatile bool isFinished;
		static time_t startTime,stopTime;
		static int pipeFD[2];
		static std::thread * thread;
		static volatile pid_t pid;
		static int childStatus;
		static int videoLength;
		static void Monitoring();

};
#endif /* _VIDEO_CONTROLS_H_ */
