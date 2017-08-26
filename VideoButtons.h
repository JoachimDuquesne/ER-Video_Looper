#include <stdint.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <list>
#include <thread>

#define PUSHED_TIMEOUT	2   // time in sec before reseting an active button
//using namespace std;

struct ButtonTypeDef
{
    int pin;
    char const * name;
    bool pushed;
    time_t time;
};

class VideoButtons
{
    public:
	VideoButtons();
	void AddButton(int gpioPin, char const * name);
	void RemoveButton(int gpioPin);
	bool HasBeenPushed(int gpioPin);
	void ResetButton(int gpioPin);
	void printButton(int gpioPin);
	
    private:
	static void Monitoring(VideoButtons * This);
	std::thread * f;
	std::list<ButtonTypeDef> buttonList;
};
