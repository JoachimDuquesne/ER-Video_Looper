 #include <stdint.h>
#include <stdlib.h>
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
#define QOS			2

#define VIDEOFILE "/home/metabaron/tableau.mp4"
//#define VIDEOFILE "/home/EscapeRush/tableau.mp4"

int main(int argc, char *argv[])
{
	
	QCoreApplication app(argc, argv);
	
	VideoMQTT mqtt(MQTT_PORT,MQTT_BROKER_IP,MQTT_TOPIC,QOS);
	VideoButtons button;
	button.AddButton(TOGGLE_BUTTON,"Toggle");
	button.AddButton(RESET_BUTTON,"Reset");
	
	while(!VideoControls::Init())
		usleep(100000);
	
//	VideoControls::Start(VIDEOFILE);
//VideoControls::Stop();
	
	//VideoControls::Play();
	
/*	VideoControls::Stop();  // Necessary when using systemd for startup
	sleep(2);
	VideoControls::Start("/home/metabaron/tableau.mp4");
	sleep(2);
	VideoControls::Pause();
*/

	while(1)
	{
/*		if(button.HasBeenPushed(TOGGLE_BUTTON))
		{
			VideoControls::Toggle();
			//button.printButton(TOGGLE_BUTTON);
		}

		if(button.HasBeenPushed(RESET_BUTTON))
		{
			VideoControls::SetPosition(8,15);
			VideoControls::Pause();
			//button.printButton(RESET_BUTTON);
		}

*/
		usleep(100000);
	}

	return 0;
}
