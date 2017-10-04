#ifndef _VIDEO_CONTROLS_H_
#define _VIDEO_CONTROLS_H_


#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <string.h>
#include <fstream>
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
#include <QCoreApplication>
#include <QtDBus>


#define UNKNOWN_ERROR 		-99
#define DBUS_FAILED			-2
#define	OMXPLAYER_OFF		-1
#define NO_ERROR			0
#define OMXPLAYER_PLAYING	1
#define OMXPLAYER_PAUSED	2


class VideoControls {
	public:
		VideoControls(){};
		static void Init();

		static void Start(char const * videofile);
		static void Stop();

		static void Pause();
		static void Play();
		static void Toggle();

		static void SetPosition(int64_t pos, int64_t length);
		static void Reset();
		
		//static void GetStatus();
		static bool GetIsPlaying();

	private:
		
		static void Monitoring();
		
		static volatile int status;
		static volatile int error;
		
		static volatile int position;
		static int endPosition;
		
		static std::thread * thread;
		static volatile pid_t pid;
		static int childStatus;
		
		static QDBusConnection * dbus;
		static QDBusInterface * OMXRootIface;
		static QDBusInterface * OMXPlayerIface;
		static QDBusInterface * OMXPropertiesIface;
		
};
#endif /* _VIDEO_CONTROLS_H_ */
