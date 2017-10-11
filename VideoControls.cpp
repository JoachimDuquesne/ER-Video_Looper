
#include "VideoControls.h"


bool VideoControls::Init()
{	
	setenv("OMXPLAYER_DBUS_ADDR", "\"/tmp/omxplayerdbus.pi\"", true);
	setenv("OMXPLAYER_DBUS_PID",  "\"/tmp/omxplayerdbus.pi.pid\"", true);
	
	std::ifstream ifs("/tmp/omxplayerdbus.pi");
	std::string content;
	getline(ifs,content);
	setenv("DBUS_SESSION_BUS_ADDRESS", content.c_str(), true);
	
	std::ifstream ifs2("/tmp/omxplayerdbus.pi.pid");
	std::string content2;
	getline(ifs2,content2);
	setenv("DBUS_SESSION_BUS_PID", content2.c_str(), true);

	setenv("DISPLAY", ":0", true); // X must be running
	
	dbus  = new QDBusConnection(QDBusConnection::sessionBus());
	if (!dbus->isConnected())
	{
		qCritical() << "Cannot connect to the D-Bus session bus."  << dbus->lastError() << "\n";
    	return false;
    }
    error = OMXPLAYER_OFF;
    status = NO_ERROR; // if error != NO_ERROR : status must be disregarded
    dbus_active=true; // So by default, monitoring doesnt send command to DBUS
	thread = new std::thread(Monitoring);  // Constructor are called before entering main()... Threads must be created later..
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void VideoControls::Start(char const * videofile)
{
	if(pid >0) // A child already exist
		Stop();
		
	if(error != OMXPLAYER_OFF)
		exit(EXIT_FAILURE);
	
	pid = fork();	// Spawn child process

	if(pid == -1)
	{// Check if spawn
		perror("Error forking omxplayer");
		exit(EXIT_FAILURE);
	}

	// Child process
	if(pid == 0)
	{
		char const * arguments[] = {"omxplayer","--no-osd","-o","hdmi","--aspect-mode","fill",videofile,NULL};
//		char const * arguments[] = {"omxplayer","--no-osd","--display=5","--aspect-mode","fill",videofile,NULL};

		if( execv("/usr/bin/omxplayer",const_cast<char**>(arguments)) == -1)
		{
 			perror("execv omxplayer");
			exit(EXIT_FAILURE);
		}
		
		// Should never reach here !!
		error = UNKNOWN_ERROR; // Error unknown
		fprintf(stdout,"OMXPlayer crashed\n");
		
		exit(EXIT_FAILURE);
	}
	
	// As from here, we are in the father process
	sleep(2); // Need to let omxplayer start or we wont find it on DBus
	VideoControls::Init();
	if(error == UNKNOWN_ERROR) // omxplayer crashed
	{
		fprintf(stdout,"OMXPlayer crashed at startup\n");
		exit(EXIT_FAILURE);
	}
		
	OMXRootIface = new QDBusInterface("org.mpris.MediaPlayer2.omxplayer","/org/mpris/MediaPlayer2","org.mpris.MediaPlayer2");
	OMXPlayerIface = new QDBusInterface("org.mpris.MediaPlayer2.omxplayer","/org/mpris/MediaPlayer2","org.mpris.MediaPlayer2.Player");
	OMXPropertiesIface = new QDBusInterface("org.mpris.MediaPlayer2.omxplayer","/org/mpris/MediaPlayer2","org.freedesktop.DBus.Properties");
	
	if (!OMXPlayerIface->isValid())
	{
		qCritical() << "OMXPlayerIface NOT connected. "  << OMXPlayerIface->lastError() << "\n";
		error = DBUS_FAILED;
	}
			
	if (!OMXPropertiesIface->isValid())
	{
		qCritical() << "OMXPropertiesIface NOT connected. "  << OMXPropertiesIface->lastError() << "\n";
		error = DBUS_FAILED;
	}
	
	if (!OMXRootIface->isValid())
	{
		qCritical() << "OMXRootIface NOT connected. "  << OMXRootIface->lastError() << "\n";
		error = DBUS_FAILED;
	}
	
	if(error != OMXPLAYER_OFF)
	{
		kill(pid,SIGTERM);
		exit(EXIT_FAILURE);
	}

	error = NO_ERROR;
	dbus_active=false;
	SetPosition(0,1);
    SetPosition(0,1);
}

void VideoControls::Stop()
{
	while(dbus_active);
    dbus_active=true;
    OMXPlayerIface->call(QDBus::Block,"Stop");
    usleep(DBUS_DELAY);
    dbus_active=false;
	
    error = OMXPLAYER_OFF;
	wait(&childStatus);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void VideoControls::Play()
{
	if(error != NO_ERROR) 
	{
		fprintf(stdout,"Error :%d\n",status);
		return;
	}

	if(position > endPosition)
	{
		fprintf(stdout,"Video end reached\n");
		return;
	}
		
	if(status == OMXPLAYER_PLAYING)
		fprintf(stdout,"Video already Playing\n");

    while(dbus_active);
    dbus_active=true;
	OMXPlayerIface->call(QDBus::Block,"Play");
    usleep(DBUS_DELAY);
    dbus_active=false;
}

void VideoControls::Pause()
{
	if(error != NO_ERROR) 
	{
		fprintf(stdout,"Error :%d\n",status);
		return;
	}
	
	if(status == OMXPLAYER_PAUSED)	
		fprintf(stdout,"Video already plaused\n");
    
    
	while(dbus_active);
    dbus_active=true;
	OMXPlayerIface->call(QDBus::Block,"Pause");
    usleep(DBUS_DELAY);
    dbus_active=false;
}

void VideoControls::Toggle()
{
	if(status == OMXPLAYER_PLAYING)
		Pause();
	else
		Play();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void VideoControls::SetPosition(int64_t pos, int64_t length)
{
	if(error != NO_ERROR) 
	{
		fprintf(stdout,"Error :%d\n",status);
		return;
	}

	//Pause();
    //sleep(1);
	
	while(dbus_active);
    dbus_active=true;
	OMXPlayerIface->call(QDBus::Block,"SetPosition",pos*1000000); // OMXplayer expect time in microseconds, we use seconds
	endPosition = (pos)*1000000 + length*1000000;
    usleep(DBUS_DELAY);    
    //dbus_active=false;
    //while(dbus_active);
    //dbus_active=true;
	//OMXPlayerIface->call(QDBus::Block,"SetPosition",(pos+1)*1000000); // OMXplayer expect time in microseconds, we use seconds
	usleep(DBUS_DELAY);    
    dbus_active=false;
    
    
    Play();

}

void VideoControls::Reset()
{
	SetPosition(0,0);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool VideoControls::GetIsPlaying()
{
	return (status == OMXPLAYER_PLAYING);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void VideoControls::Monitoring()
{
	fprintf(stdout,"monitoring...\n");
	
	QDBusReply<qlonglong> Pos;
	QDBusReply<QString> Playing;
	uint32_t i=0;
	while(1) 
	{
		usleep(100000);
		i++;
		
		if(error != NO_ERROR)
		{
			if(i%10==0)
				fprintf(stderr,"err:%d",error); // error has been set
			continue;
		}
		
		while(dbus_active);
        dbus_active=true;
		Playing = OMXPropertiesIface->call(QDBus::Block,"Get","org.mpris.MediaPlayer2.Player", "PlaybackStatus");
		usleep(DBUS_DELAY);
        dbus_active=false;
        
		if(!strcmp(qPrintable(Playing.value()),"Playing"))
			status = OMXPLAYER_PLAYING;
		else if(!strcmp(qPrintable(Playing.value()),"Paused"))
			status = OMXPLAYER_PAUSED;
		else
			error = DBUS_FAILED;
 		
        while(dbus_active);
        dbus_active=true;
		Pos = OMXPropertiesIface->call(QDBus::Block,"Get","org.mpris.MediaPlayer2.Player", "Position");
        usleep(DBUS_DELAY);
        dbus_active=false;
        
        position = (int64_t)(Pos.value());
		
		
		if(i%10==0)
		{
			printf("status:%s\n",qPrintable(Playing.value()));
			printf("time:%.1f/%d\n",(float)position/1000000,endPosition/1000000);
		}
		
		printf("%.1f\n",(float)position/1000000);
        
		if(position > endPosition)
			if(status == OMXPLAYER_PLAYING)
			{
				Pause();
				fprintf(stdout,"Video Length reach, pausing... \n");
			}

//		if(i%10==0)
//			fprintf(stdout,"."); // Proof of thread running
	}
}

volatile int VideoControls::error = OMXPLAYER_OFF;;
volatile int VideoControls::status = OMXPLAYER_OFF;
volatile bool VideoControls::dbus_active = false;

volatile int VideoControls::position;
volatile int VideoControls::endPosition;

std::thread * VideoControls::thread;
volatile pid_t VideoControls::pid=0;
int VideoControls::childStatus;

QDBusConnection * VideoControls::dbus;
QDBusInterface * VideoControls::OMXRootIface;
QDBusInterface * VideoControls::OMXPlayerIface;
QDBusInterface * VideoControls::OMXPropertiesIface;


