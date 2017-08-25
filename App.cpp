// N.C. Cruz, University of Almeria, Spain (2017)
#include "App.h"

App::App(void)
	:BApplication("application/x-vnd.nccruz-Pin8"){
	this->window = new MainWindow();
	this->emuKernel = new Chip8Core(1);//By default, sound allowed
	this->emuKernel->initialize();
	this->window->linkScreen(emuKernel->getScreen());
	this->window->Show();
	this->beeper = spawn_thread(beeperFunc, "beeperThread", 120, this);
}

App::~App(void){//delete window;//Left to the environment (otherwise, an error appears once in a while when closing)
	delete emuKernel;
}

int32 App::threadFunc(void *data){
	App* application = (App*) data;
	MainWindow* window = application->window;
	Chip8Core* kernel = application->emuKernel;
	long long startTimeCore, endTimeCore, diffTime, waitInterval;
	long long startTimeTimers = App::getCurrentTime();
	while(1){
		startTimeCore = App::getCurrentTime();//For the kernel
		if ((App::getCurrentTime() - startTimeTimers) >= 16666) { // 60 Hz = 1s/60 = 1000ms/60
			startTimeTimers = App::getCurrentTime();//For the timers
            kernel->updateTimers();//Decreasing the timers
        }
        waitInterval = (window->isFastMode()?FAST_PERIOD:SLOW_PERIOD);// 540 Hz = 1s/540 = 1000ms/540 -> 1.85 ms (slow mode).//1250
        for(int i = 0; i<16; i++){
        	if(window->getButtonState(i)){
        		kernel->setKey(MainWindow::keyVals[i], true);	
        	}else{
        		kernel->setKey(MainWindow::keyVals[i], false);
        	}	
        }
		kernel->emulateCycle();
        if(kernel->shouldDraw()){
        	window->Lock();
        	window->updateScreen();
        	window->Unlock();
        }
        if(!window->isBeepBlocked() && kernel->shouldBeep()){
        	resume_thread(application->beeper);
        }
        endTimeCore = App::getCurrentTime();
        diffTime = endTimeCore - startTimeCore;
        if(diffTime < waitInterval){
        	snooze(waitInterval - diffTime);	
        }
	}
	return 0;
}

int32 App::beeperFunc(void* data){
	App* application = (App*) data;
	while(1){
		beep();
		suspend_thread(application->beeper);//Suspend my self after beeping
	}
	return 0;	
}

long long App::getCurrentTime(void){
	struct timeval tp;
    gettimeofday(&tp, NULL);
    long long currentTime = (long long) tp.tv_sec * 1000000L + tp.tv_usec;
    return currentTime;
}

bool App::QuitRequested(void){
	kill_thread(beeper);
	kill_thread(emuThread);
	return true;
}

//Default handler of the BFilePanel
void App::RefsReceived(BMessage *message){
	entry_ref ref;
	int load_status = 0;
	if(message->FindRef("refs", &ref)==B_OK){
		this->emuKernel->initialize();
		BEntry entry(&ref, true);//Access to the real file
		BFile file(&entry, B_READ_ONLY);
		if(file.InitCheck()==B_OK){
			off_t length;
			file.GetSize(&length);
			if(length<=4096){
				BPath path(&ref);
				kill_thread(emuThread);
				if(this->window->IsLocked())
					this->window->Unlock();//Just in case the emulator thread was killed before unlocking the window...
				load_status = emuKernel->loadApplication(path.Path());
			}
		}
	}
	if(load_status != 1){
		BAlert* errorMessage = new BAlert("Error", "The file cannot be accessed or it is too big for a proper Chip-8 game.", "Ok");
		errorMessage->Go();
	}else{
		this->window->Activate();
		this->emuThread = spawn_thread(threadFunc, "emuThread", 120, this);
		resume_thread(this->emuThread);
	}
}

int main(void){
	App* app = new App();
	app->Run();
	delete app;
	return 0;	
}
