
#include "VideoControls.h"


VideoControls::VideoControls()
{
// Implement singelton
}

void VideoControls::Init()
{
		//if(thread != NULL) // Launching thread failed
			thread = new std::thread(Monitoring);
}


// Start omxplayer
void VideoControls::Start(char const * videofile, int videolength)
{

	if(pid >0) // A child already exist
		Stop();

	pipe(pipeFD);	// Create a pipe between father and child process
	pid = fork();	// Spawn child process

	if(pid == -1)
	{// Check if spawn
		perror("Error forking omxplayer");
		exit(EXIT_FAILURE);
	}

	// Father process
	if(pid > 0)
	{
		startTime = time(NULL);
		stopTime  = time(NULL);
		videoLength = videolength;
		close(pipeFD[0]); // closing the reading side of the pipe
		isPlaying = true;
		isFinished = false;
	}

	// Child process
	if(pid == 0)
	{
		close(pipeFD[1]);	// closing write side of the pipe
		dup2(pipeFD[0],STDIN_FILENO); // Linking pipe output to stdin (read by omxplayer)
		char const * arguments[] = {"omxplayer","--no-osd","-o","hdmi",videofile,NULL};

		if( execv("/usr/bin/omxplayer",const_cast<char**>(arguments)) == -1)
		{
 			perror("execv omxplayer");
			exit(EXIT_FAILURE);
		}
		// If the child reach here, that means we have send 'quit' to omxplayer to we kill the child process
		// or omxplayer exited for another reason
		Stop();
		exit(EXIT_SUCCESS);
	}
}


// Toggle between play and paused
// Update isPlaying, startTime and stopTime
void VideoControls::Toggle()
{
	if(pid <= 0) // We dont have a child running omxplayer so we can't toggle
	{
		fprintf(stdout,"No video playing, can't toggle\n");
		return;
	}
	// If the file is paused and finished, don't toggle it
	if(!isPlaying && isFinished)
	{
		fprintf(stdout,"File is finished\n");
		return;
	}

	write(pipeFD[1],"p",1);
	fsync(pipeFD[1]);

	if(isPlaying)
		stopTime = time(NULL); // if video was playing and is being paused, note the stopTime
	else // We compute a new startTime disregarding the pause
		startTime = time(NULL) - (stopTime-startTime);


	isPlaying = !isPlaying;

	if(isPlaying)
		fprintf(stdout,"play\n");
	else
	 	fprintf(stdout,"pause\n");

}


void VideoControls::Pause()
{
	if(isPlaying)
		Toggle();
}


void VideoControls::Play()
{
	if(!isPlaying)
		Toggle();
}


// Restart the video
void VideoControls::Reset()
{
	if(pid <= 0)
	{
		fprintf(stdout,"No video playing, can't reset\n");
		return;
	}

	fprintf(stdout,"Reseting the video \n");

	Pause();
	usleep(100000);

	write(pipeFD[1],"i",1); // return to the previous chapter
	fsync(pipeFD[1]);
	usleep(100000); // Give time to omxplayer to restart the video
		  // ABSOLUTELY NEEDED !!!!
	startTime = time(NULL); // reseting the chrono
	stopTime  = time(NULL);
	isFinished = false;

	Play();
	sleep(1);
}


bool VideoControls::GetIsPlaying()
{
	return isPlaying;
}

bool VideoControls::GetIsFinished()
{
	return isFinished;
}


void VideoControls::Stop()
{
	write(pipeFD[1],"q",1);
	fsync(pipeFD[1]);
	wait(&childStatus);
	pid=0;
	isPlaying = false;
	isFinished = true;
	close(pipeFD[1]);
}


void VideoControls::Monitoring()
{
	fprintf(stdout,"monitoring...\n");
	sleep(1);

	while(1) // while omxplayer is running
	{
		sleep(1);
	
		if(pid<=0) // omxplayer not running
			continue;
			
		fprintf(stderr,".");

		if( isPlaying && (time(NULL) > startTime+videoLength) )
		{
			isFinished = true;
			Pause(); // pause the video
			fprintf(stdout,"Video Length reach, pausing... \n");
		}	
	}
}

volatile bool VideoControls::isPlaying;
volatile bool VideoControls::isFinished;
time_t VideoControls::startTime,VideoControls::stopTime;
int VideoControls::pipeFD[2];
std::thread * VideoControls::thread;
volatile pid_t VideoControls::pid;
int VideoControls::childStatus;
int VideoControls::videoLength;
