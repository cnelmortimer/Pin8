// N.C. Cruz, University of Almeria, Spain (2017)
#ifndef MAIN_WINDOW
#define MAIN_WINDOW

//#include <Application.h>//To get the definition of be_app_messenger
#include <Window.h>
#include <Button.h>
#include <CheckBox.h>
#include <StringView.h>
#include <FilePanel.h>//Do not forget to link libtracker.so!!
#include "Screen.h"

#define DEFAULT_PIXEL 6
#define INIT_TOP_LEFT 50
#define INIT_RIGHT 434
#define INIT_BOTTOM 440	
#define BUT_LOAD 17			//Number linked to loading a rom (not using enum's)

class MainWindow: public BWindow{
	
	public:
		MainWindow(void);
		~MainWindow(void);
		
		void linkScreen(const bool* status);
		void updateScreen(void) const;
		bool isBeepBlocked(void) const;
		bool isFastMode(void) const;
		bool getButtonState(int index) const;
		
		static const char* keyNames[]; //= "123C456D789EA0BF";
		static const int keyVals[];//{1, 2, 3, 12, 4, 5, 6, 13, 7, 8, 9, 14, 10, 0, 11, 15};
				
	private:
		static int PIXEL_SIZE;	// Size in real pixels of every pixel of Chip-8
		
		//Attributes:
		Screen* screen;
		BButton* openButton;
		BFilePanel* openPanel;
		BButton** chipBut;
		BCheckBox* soundCheck; 
		BCheckBox* fastModeCheck; 
		BView* authorLabel;
		
		//Internal Auxiliary Methods:
		void MessageReceived(BMessage* msg);
		
		void getButtonSize(int& butWidth, int& butHeight);
		void getLabelSize(const char* msg, int& butWidth, int& butHeight);
		
		void initializeUI(void);
		
		void FrameResized(float newWidth, float newHeight);//Hook to react to resizing
		
		void adjustScreen(void);
		void adjustControls(void);
};

#endif
