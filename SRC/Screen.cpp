#include "Screen.h"

const rgb_color Screen::black = {0, 0, 0, 255};
const rgb_color Screen::white = {255, 255, 255, 255};

Screen::Screen(BRect rect, char* name, BButton** buttonSet, int PIXEL_SIZE)
	:BView(rect, name, B_FOLLOW_NONE, B_WILL_DRAW){
	this->PIXEL_SIZE = PIXEL_SIZE;
	this->buttonSet = buttonSet;
	this->status = 0;
}

void Screen::setStatus(const bool* status){
	this->status = status;
}

void Screen::setPixelSize(int pixelSize){
	this->PIXEL_SIZE = pixelSize;
}

void Screen::Draw(BRect updateRect){
	if(status){
		int i = 0, x = 0, y = 0;
		for(int ver = 0; ver<32; ver++){
			x=0;
			for(int hor = 0; hor<64; hor++){
				if(this->status[i])
					this->SetHighColor(white);
				else
					this->SetHighColor(black);
				this->FillRect(BRect(x, y, x+PIXEL_SIZE, y+PIXEL_SIZE), B_SOLID_HIGH);
				i++;
				x+=PIXEL_SIZE;
			}
			y+=PIXEL_SIZE;
		}
	}
}

void Screen::KeyDown(const char* bytes, int32 numBytes){
	this->manageKeys(*bytes, true);
}

void Screen::KeyUp(const char* bytes, int32 numBytes){
	this->manageKeys(*bytes, false);
}

void Screen::manageKeys(char name, bool val){
	int index = -1;
	switch(name){
		case '0': index = 13;	break;
		case '1': index = 0; 	break;
		case '2': index = 1; 	break;
		case '3': index = 2; 	break;
		case '4': index = 4; 	break;
		case '5': index = 5; 	break;
		case '6': index = 6; 	break;
		case '7': index = 8; 	break;
		case '8': index = 9; 	break;
		case '9': index = 10;	break;
		case 'a': index = 12; 	break;
		case 'b': index = 14; 	break;
		case 'c': index = 3; 	break;
		case 'd': index = 7; 	break;
		case 'e': index = 11; 	break;
		case 'f': index = 15; 	break;
	}
	if(index>-1){
		this->buttonSet[index]->SetValue(val);
	}
}
