#include "VideoControls.h"


VideoControls::VideoControls()
{
// Implement singelton
}


// Start omxplayer
void VideoControls::startVideo(char const * VideoFile, unsigned int Video_Length)
{
	VideoLength = Video_Length;
	isPlaying = true;

	if(pid >0) // A child already exist
		stopVideo();

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
		dup2(pipeFD[0],STDIN_FILENO);
		char const * arguments[] = {"omxplayer","--no-osd","-o","hdmi",VideoFile,NULL};
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
	close(pipeFD[0]); // closing the reading side of the pipe
}


// Toggle between play and paused
// Update isPlaying, startTime and stopTime
void VideoControls::toggleVideo()
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
void VideoControls::resetVideo()
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
	    toggleVideo();
}

bool VideoControls::getIsPlaying()
{
	return isPlaying;
}

bool VideoControls::getIsFinished()
{
	return (time(NULL) >= startTime + VideoLength)? true : false ;
}

bool VideoControls::process(uint8_t cmd, char value[])
{
	if(getIsFinished())
		stopVideo();

	switch(command[1])
	{
	case START: startVideo(VideoFile,VideoLength);
				break;
			  
	case PAUSE: toggleVideo();
				break;
			  
	case RESET: resetVideo();
				break;
			  
	case STOP : stopVideo();
				break;
			  
	case QUIT : stopVideo();
				int status;
				wait(&status);
				return false;
				break;
		
	//case NEWFILE :	setVideoFile(value)
	}
	
	return true;
}


void VideoControls::stopVideo()
{
	write(pipeFD[1],"q",1);
	fsync(pipeFD[1]);

	pid=0; // reset pid
	isPlaying = false;
	close(pipeFD[1]);
}
