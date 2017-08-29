// N.C. Cruz, University of Almeria, Spain (2017)
#ifndef APP_H
#define APP_H

#include <Application.h>
#include <Alert.h>
#include <Beep.h>
#include <File.h>
#include <Path.h>
#include <sys/time.h>
#include "Core/Chip8Core.h"
#include "MainWindow.h"

#define SLOW_PERIOD 1852
#define FAST_PERIOD 1000

class App: public BApplication{
	public:
		App(void);
		~App(void);		
	private:
		//Attributes:
		MainWindow* window;
		Chip8Core* emuKernel;
		thread_id emuThread;
		thread_id beeper;
		
		//Internal auxiliary functions:
		static int32 threadFunc(void *data);
		static long long getCurrentTime(void);
		static int32 beeperFunc(void* data);
		
		bool QuitRequested(void);
		void RefsReceived(BMessage *message);
};

#endif
