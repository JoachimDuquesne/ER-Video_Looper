#include "VideoButtons.h"


VideoButtons::VideoButtons()
{
	wiringPiSetupGpio();
	thread = new std::thread(Monitoring,this);
}

void VideoButtons::AddButton(int gpioPin, char const * name)
{
	ButtonTypeDef button = {gpioPin,name,false,time(NULL)};
	pinMode(gpioPin,INPUT);
	pullUpDnControl(gpioPin, PUD_DOWN);
	buttonList.push_back(button);
}

void VideoButtons::RemoveButton(int gpioPin)
{
	std::list<ButtonTypeDef>::iterator it;

	for(it=buttonList.begin(); it!=buttonList.end() ; it++)
		if(it->pin == gpioPin)
			break;

	buttonList.erase(it);
}

bool VideoButtons::HasBeenPushed(int gpioPin)
{
	std::list<ButtonTypeDef>::iterator it;

	for(it=buttonList.begin(); it!=buttonList.end() ; it++)
		if(it->pin == gpioPin)
			break;
	
	if(it->pushed)
	{
		ResetButton(it->pin);
		return true;
	}
	return false;
}

void VideoButtons::ResetButton(int gpioPin)
{
	std::list<ButtonTypeDef>::iterator it;

	for(it=buttonList.begin(); it!=buttonList.end() ; it++)
		if(it->pin == gpioPin)
			break;

	it->pushed = false;
}

void VideoButtons::printButton(int gpioPin)
{
	std::list<ButtonTypeDef>::iterator it;

	for(it=buttonList.begin(); it!=buttonList.end() ; it++)
		if(it->pin == gpioPin)
			break;

	printf("%s\n",it->name);
}

void VideoButtons::Monitoring(VideoButtons * This)
{
    std::list<ButtonTypeDef>::iterator it;

    while(1)
    {
    	usleep(1000);
    	
		for(it=This->buttonList.begin(); it!=This->buttonList.end(); it++)
		{
			if(digitalRead(it->pin))
			{
				usleep(20000);
				if(digitalRead(it->pin))
				{
					while(digitalRead(it->pin))
						usleep(100);

					it->pushed = true;
					it->time = time(NULL);
				}
			}

			if(it->time + PUSHED_TIMEOUT < time(NULL))
				it->pushed = false;
		}
    }
}

