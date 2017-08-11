
#include "VideoControls.h"


VideoControls::VideoControls()
{
	if(pthread_create(&thread, NULL, monitoring, NULL) == -1) // Launching thread failed
	{
		perror("pthread_create");
		exit(EXIT_FAILURE);
	}

// Implement singelton
}


// Start omxplayer
void VideoControls::start(char const * videofile, int videolength)
{

	if(pid >0) // A child already exist
	{
		stop();
		//wait for child to terminate
	}

	pipe(pipeFD);	// Create a pipe between father and child process
	pid = fork();	// Spawn child process

	if(pid == -1)
	{// Check if spawn
		perror("Error forking omxplayer");
		exit(EXIT_FAILURE);
	}


	if(pid == 0)
	{ // We are in the child process : start omxxplayer
		close(pipeFD[1]);	// closing write side of the pipe
		dup2(pipeFD[0],STDIN_FILENO); // Linking pipe output to stdin (read by omxplayer)
		char const * arguments[] = {"omxplayer","--no-osd","-o","hdmi",videofile,NULL};
		if( execv("/usr/bin/omxplayer",const_cast<char**>(arguments)) == -1)
		{
			perror("execv omxplayer");
			exit(EXIT_FAILURE);
		}
		// If the child reach here, that means we have send 'quit' to omxplayer to we kill the child process
		exit(EXIT_SUCCESS);
	}
	// Only the father process can arrive here
	startTime = time(NULL);
	stopTime  = time(NULL);
	videoLength = videolength;
	close(pipeFD[0]); // closing the reading side of the pipe
	isPlaying = true;
	isFinished = false;
}


// Toggle between play and paused
// Update isPlaying, startTime and stopTime
void VideoControls::toggle()
{
	if(pid <= 0) // We dont have a child running omxplayer so we can't toggle
	{
		fprintf(stdout,"No video playing, can't toggle\n");
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


// Restart the video
void VideoControls::reset()
{
	if(pid <= 0)
	{
		fprintf(stdout,"No video playing, can't reset\n");
		return;
	}

	fprintf(stdout,"Reseting the video \n");

	write(pipeFD[1],"i",1); // return to the previous chapter
	fsync(pipeFD[1]);

	startTime = time(NULL); // reseting the chrono
	stopTime  = time(NULL);


	// If the video was paused before reseting, launch it
	if(!isPlaying)
	    toggle();
}


bool VideoControls::getIsPlaying()
{
	return isPlaying;
}

bool VideoControls::getIsFinished()
{
	return isFinished;
}


void VideoControls::stop()
{
	write(pipeFD[1],"q",1);
	fsync(pipeFD[1]);

	pid=0; // reset pid
	isPlaying = false;
	close(pipeFD[1]);
}


void * VideoControls::monitoring(void *arg)
{
	while(!isFinished)
	{
		if(isPlaying)
			isFinished = (time(NULL) > startTime + videoLength) ? true : false ;
		sleep(1);
	}
	fprintf(stdout,"Video Length reach, stopping... \n");
	stop();
	pthread_exit(NULL);
}

bool VideoControls::isPlaying;
bool VideoControls::isFinished;
time_t VideoControls::startTime,VideoControls::stopTime;
int VideoControls::pipeFD[2];
pthread_t VideoControls::thread;
pid_t VideoControls::pid;
int VideoControls::videoLength;
void * VideoControls::monitoring(void * arg);
