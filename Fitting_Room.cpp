 #include <stdint.h>
#include <stdlib.h>
//#include <dbus/dbus.h>

#include "VideoControls.h"
#include "VideoButtons.h"
#include "VideoMQTT.h"

#define TOGGLE_BUTTON	20
#define RESET_BUTTON	21

#define MQTT_PORT	1883
//#define MQTT_BROKER_IP	"192.168.0.50"  // At home
#define MQTT_BROKER_IP	"127.0.0.1"	// Loopback
//#define MQTT_BROKER_IP	"192.168.43.1"	 // Android hotspot
#define MQTT_TOPIC	"VideoControlTopic"
#define QOS			2


int main(int argc, char *argv[])
{
	VideoMQTT mqtt(MQTT_PORT,MQTT_BROKER_IP,MQTT_TOPIC,QOS);
	VideoButtons button;jou
	button.AddButton(TOGGLE_BUTTON,"Toggle");
	button.AddButton(RESET_BUTTON,"Reset");
	VideoControls::Init();
	
	VideoControls::Start("/home/metabaron/tableau.mp4",90);
	sleep(2);
	VideoControls::Reset();
	sleep(1);
	VideoControls::Pause();

	while(1)
	{
		if(button.HasBeenPushed(TOGGLE_BUTTON))
		{
			VideoControls::Toggle();
			button.printButton(TOGGLE_BUTTON);
		}

		if(button.HasBeenPushed(RESET_BUTTON))
		{
			VideoControls::Reset();
			button.printButton(RESET_BUTTON);
		}

		if(VideoControls::GetIsFinished())
		{
			//VideoControls::Reset();
			VideoControls::Pause();
		}

		usleep(100000);
	}

	return 0;
}
