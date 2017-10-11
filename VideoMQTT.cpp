//============================================================================
// Name        : Escape_Rush_Entrance_Room_Video_Looper.cpp
// Author      : Joachim
// Version     :
// Copyright   : Gift to Escape Rush SPRL ;)
// Description :
//============================================================================


#include "VideoMQTT.h"


VideoMQTT::VideoMQTT(int Port, const char * Broker_IP, const char * Topic, int qos) : port(Port),broker_IP(Broker_IP),topic(Topic),QoS(qos)
{
/*	Port	  = port;
	IP_Broker = IP_broker;
	Topic	  = topic;
*/
	mosquitto_lib_init();
	
	mosq = mosquitto_new(NULL,true,NULL);
	if(mosq == NULL)
	{
		perror("mosquitto_new:");
		exit(EXIT_FAILURE);
	}

	mosquitto_message_callback_set(mosq,ParseMessage);
	
	if(mosquitto_connect(mosq,broker_IP,port,60) != 0)
	{
		perror("mosquitto connect:");
		exit(EXIT_FAILURE);
	}

	if(mosquitto_subscribe(mosq,&messageID,topic,QoS) != 0)
	{
		perror("mosquitto subscribe:");
		exit(EXIT_FAILURE);
	}
	
	f = new std::thread(Monitoring,this);
	newMessage = false;
	
}

void VideoMQTT::ParseMessage(struct mosquitto * mosq, void * userdata, const struct mosquitto_message * message)
{
	std::string s = (char*)message->payload;
	s.append(" ");
	size_t pos = 0;
	std::string cmd[5];
	uint8_t	i=0;

	fprintf(stdout,"%s\n",s.c_str());

	while( (pos=s.find(" ")) != std::string::npos)
	{
		cmd[i]=s.substr(0,pos);
		s.erase(0,pos+1);
		//fprintf(stderr,"%s ",cmd[i].c_str());
		i++;
	}
	//fprintf(stderr,"\n");

	if(!strcmp("start",cmd[0].c_str()))
		if(i == 2) // We need one argument
			VideoControls::Start(cmd[1].c_str());
		
	if(!strcmp("setPosition",cmd[0].c_str()))
		if(i == 3) // We need two argument
			VideoControls::SetPosition(atoi(cmd[1].c_str()),atoi(cmd[2].c_str()));

	if(!strcmp("stop",cmd[0].c_str()))
		VideoControls::Stop();

	if(!strcmp("reset",cmd[0].c_str()))
		VideoControls::Reset();

	if(!strcmp("pause",cmd[0].c_str()))
		VideoControls::Toggle();
}

void VideoMQTT::SendMessage(const char * msg)
{
	
}


void VideoMQTT::Monitoring(VideoMQTT * This)
{
	while(1)
	{
		mosquitto_loop(This->mosq,300,1);
		usleep(100000);
	}
}
