// N.C. Cruz, University of Almeria, Spain (2017)

#include "MainWindow.h"

const char* MainWindow::keyNames[] = {"1","2","3","C","4","5","6","D","7","8","9","E","A","0","B","F"};
const int MainWindow::keyVals[] = {1, 2, 3, 12, 4, 5, 6, 13, 7, 8, 9, 14, 10, 0, 11, 15};
int MainWindow::PIXEL_SIZE = DEFAULT_PIXEL;//By default

MainWindow::MainWindow(void)
	: BWindow(BRect(INIT_TOP_LEFT, INIT_TOP_LEFT, INIT_RIGHT, INIT_BOTTOM), "Pin8", B_TITLED_WINDOW, B_ASYNCHRONOUS_CONTROLS | B_QUIT_ON_WINDOW_CLOSE){
	this->PIXEL_SIZE = (int) this->Bounds().Width()/64;//It is 6 by default
	this->initializeUI();
	//Adjust the layout for the frst time:
	this->adjustScreen();
	this->adjustControls();
}

MainWindow::~MainWindow(void){
	delete this->openPanel; 	
}

void MainWindow::linkScreen(const bool* status){
	this->screen->setStatus(status);
}

void MainWindow::updateScreen(void) const{
	this->screen->Invalidate();
}

bool MainWindow::isBeepBlocked(void) const{
	return (this->soundCheck->Value()==B_CONTROL_ON);	
}

bool MainWindow::isFastMode(void) const{
	return (this->fastModeCheck->Value()==B_CONTROL_ON);	
}

bool MainWindow::getButtonState(int index) const{
	return (this->chipBut[index]->Value()==B_CONTROL_ON);
}

void MainWindow::MessageReceived(BMessage* msg){
	if(msg->what == BUT_LOAD){
		this->openPanel->Show();
	}else{
		BWindow::MessageReceived(msg);
	}
}

void MainWindow::getButtonSize(int& butWidth, int& butHeight){
	BButton* tmp = new BButton(BRect(0, 0, 1, 1), "tmp", "0", 0);
	tmp->ResizeToPreferred();
	butWidth = tmp->Bounds().IntegerWidth();
	butHeight = tmp->Bounds().IntegerHeight();
	delete tmp;
}

void MainWindow::getLabelSize(const char* msg, int& butWidth, int& butHeight){
	BStringView* tmp = new BStringView(BRect(0, 0, 1, 1), "tmp", msg);
	tmp->ResizeToPreferred();
	butWidth = tmp->Bounds().IntegerWidth();
	butHeight = tmp->Bounds().IntegerHeight();
	delete tmp;
}

void MainWindow::initializeUI(void){
	//Prepare the keyboard!
	this->chipBut = new BButton*[16];
	for(int i = 0; i<16; i++){
		this->chipBut[i] = new BButton(BRect(0, 0, 1, 1), keyNames[i], keyNames[i], new BMessage(keyVals[i]));
		this->chipBut[i]->ResizeToPreferred();
		this->chipBut[i]->SetTarget(this);//---------------
		this->AddChild(this->chipBut[i]);
	}
	//Load button:
	this->openButton = new BButton(BRect(0, 0, 1, 1), "open", "LOAD", new BMessage(BUT_LOAD));
	this->openButton->ResizeToPreferred();
	this->openButton->MakeDefault(true);
	this->AddChild(this->openButton);
	this->openPanel = new BFilePanel(B_OPEN_PANEL, 0, 0, 0, false);//Default target: be_app_messenger
	//Sound checkbox:
	this->soundCheck = new BCheckBox (BRect(0, 0, 1, 1), "soundCheck", "Beep off", 0);
	this->soundCheck->ResizeToPreferred();
	this->AddChild(this->soundCheck);
	//Fast mode checkbox:
	this->fastModeCheck = new BCheckBox (BRect(0, 0, 1, 1), "fastModeCheck", "Fast Mode", 0);
	this->fastModeCheck->ResizeToPreferred();
	this->AddChild(this->fastModeCheck);
	//Author's label:
	this->authorLabel = new BStringView(BRect(0, 0, 1, 1), "authorLabel", "N.C. Cruz (UAL), 2017");
	this->authorLabel->ResizeToPreferred();
	this->AddChild(this->authorLabel);
	//Prepare the screen:
	this->screen = new Screen(BRect(0, 0, 64*PIXEL_SIZE, 32*PIXEL_SIZE), "screen", this->chipBut, PIXEL_SIZE);
	this->AddChild(this->screen);
	this->screen->MakeFocus();//To detect keys
}

void MainWindow::FrameResized(float newWidth, float newHeight){
	this->PIXEL_SIZE = (int) newWidth/64;
	this->screen->setPixelSize(this->PIXEL_SIZE);
	this->adjustScreen();
	this->adjustControls();
}

void MainWindow::adjustControls(void){	
	//Chip-8 keyboard:
	int butWidth = 0, butHeight = 0, x = 0, y = 0, i = 0;
	this->getButtonSize(butWidth, butHeight);
	int minX = (this->Bounds().IntegerWidth())/2 - (2*butWidth) - PIXEL_SIZE - PIXEL_SIZE/2;
	y = 32*PIXEL_SIZE + PIXEL_SIZE;
	for(int j = 0; j<4; j++){
			x = minX;
			for(int z = 0; z<4; z++){
				chipBut[i]->MoveTo(x, y);
				i++;
				x += (butWidth + PIXEL_SIZE);
			}
			y += butHeight + PIXEL_SIZE;
	}
	//Load button:
	this->openButton->MoveTo(minX, y);
	//Sound off check:
	this->soundCheck->MoveTo(minX + 2*(butWidth + PIXEL_SIZE), y);
	//Fast mode checkbox:
	this->fastModeCheck->MoveTo(minX + 3*(butWidth + PIXEL_SIZE), y);
	//Author's label:
	this->getLabelSize("N.C. Cruz (UAL), 2017", butWidth, i);//We want to keep the height of the buttons
	this->authorLabel->MoveTo((this->Bounds().IntegerWidth() - butWidth)/2, 33*PIXEL_SIZE + 4*(butHeight + PIXEL_SIZE) + butHeight);//36*PIXEL_SIZE + 5*butHeight
}

void MainWindow::adjustScreen(void){
	int newWidth = 64*PIXEL_SIZE;
	this->screen->ResizeTo(newWidth, 32*PIXEL_SIZE);
	int scrWidth = this->Bounds().IntegerWidth();
	if(scrWidth>newWidth)
		this->screen->MoveTo((scrWidth - newWidth)/2, 0);
}
