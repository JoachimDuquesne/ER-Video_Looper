


#include <mosquitto.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <thread>
//#include <sys/wait.h>
//#include <fcntl.h>
//#include <time.h>
//#include <signal.h>
#include <errno.h>

#include "VideoControls.h"

class VideoMQTT : VideoControls
{
	public:
		VideoMQTT(int Port, const char * Broker_IP, const char * Topic, int qos);
		void SendStatus();
		
	private:
		static void ParseMessage(struct mosquitto * mosq, void * userdata, const struct mosquitto_message * message);
		void SendMessage(const char * msg);
		static void Monitoring(VideoMQTT * This);
		
		struct mosquitto * mosq = NULL;
		std::thread * f;
		bool newMessage;
		int messageID;
		const int port;
		const char * broker_IP;
		const char * topic;
		const int QoS;
};
