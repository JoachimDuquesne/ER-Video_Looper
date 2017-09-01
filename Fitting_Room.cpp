 #include <stdint.h>
#include <stdlib.h>
//#include <dbus/dbus.h>

#include "VideoControls.h"
#include "VideoButtons.h"

#define TOGGLE_BUTTON	20
#define RESET_BUTTON	21

//using namespace std;

int main(int argc, char *argv[])
{
	VideoControls controls;
	VideoButtons button;
	button.AddButton(TOGGLE_BUTTON,"Toggle");
	button.AddButton(RESET_BUTTON,"Reset");

	controls.start("/home/metabaron/Video1.mp4",10);
	sleep(2);
	controls.reset();
//	sleep(1);
	controls.pause();

	while(1)
	{
		if(button.HasBeenPushed(TOGGLE_BUTTON))
		{
			controls.toggle();
			button.printButton(TOGGLE_BUTTON);
		}

		if(button.HasBeenPushed(RESET_BUTTON))
		{
			controls.reset();
//			sleep(1);
			controls.pause();
			button.printButton(RESET_BUTTON);
		}

		if(controls.getIsFinished())
		{
			controls.reset();
			sleep(1);
			controls.pause();
		}

		usleep(100000);
	}

	return 0;
}
