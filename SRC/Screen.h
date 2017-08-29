#ifndef SCREEN_MODULE
#define SCREEN_MODULE

#include <View.h>
#include <Button.h>

class Screen: public BView{
		public:		
			Screen(BRect rect, char* name, BButton** buttonSet, int PIXEL_SIZE);
			
			void Draw(BRect updateRect);
			
			void setStatus(const bool* status);
			void setPixelSize(int pixelSize);
			
		private:
			static const rgb_color black;			
			static const rgb_color white;
			
			//Attributes
			BButton** buttonSet;
			const bool* status;			
			int PIXEL_SIZE;
						
			//Internal methods:
			void KeyDown(const char* bytes, int32 numBytes);
			void KeyUp(const char* bytes, int32 numBytes);
			void manageKeys(char name, bool val);
};

#endif
