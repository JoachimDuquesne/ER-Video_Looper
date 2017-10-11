#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <termios.h>

#include <QCoreApplication>
#include <QtDBus>

#include "VideoControls.h"
#include "VideoButtons.h"
#include "VideoMQTT.h"

#define TOGGLE_BUTTON	20
#define RESET_BUTTON	21

#define MQTT_PORT	1883
#define MQTT_BROKER_IP	"192.168.1.100"  // At EscapeRush
//#define MQTT_BROKER_IP "192.168.0.5"	// At home
//#define MQTT_BROKER_IP	"127.0.0.1"	// Loopback
//#define MQTT_BROKER_IP	"192.168.43.1"	 // Android hotspot
#define MQTT_TOPIC	"VideoControlTopic"
#define QOS			0

//#define VIDEOFILE "/home/metabaron/tableau.mp4"
#define VIDEOFILE "/home/pi/Videos/code.mp4"

int main(int argc, char *argv[])
{
	sleep(5);

	QCoreApplication app(argc, argv);

	VideoMQTT mqtt(MQTT_PORT,MQTT_BROKER_IP,MQTT_TOPIC,QOS);
    
//	VideoButtons button;
//	button.AddButton(TOGGLE_BUTTON,"Toggle");
//	button.AddButton(RESET_BUTTON,"Reset");
	

	
	VideoControls::Start(VIDEOFILE);
//	VideoControls::SetPosition(0,10);

while(1)
	{

		usleep(10000);
	}

	return 0;
}
