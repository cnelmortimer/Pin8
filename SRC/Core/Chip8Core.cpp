#include "Chip8Core.h"

uint8_t Chip8Core::fontSet[80] = {//[COW, MUL]:
	0xF0, 0x90, 0x90, 0x90, 0xF0, //0
	0x20, 0x60, 0x20, 0x20, 0x70, //1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
	0x90, 0x90, 0xF0, 0x10, 0x10, //4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
	0xF0, 0x10, 0x20, 0x40, 0x40, //7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
	0xF0, 0x90, 0xF0, 0x90, 0x90, //A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
	0xF0, 0x80, 0x80, 0x80, 0xF0, //C
	0xE0, 0x90, 0x90, 0x90, 0xE0, //D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
	0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};

//Public methods:

Chip8Core::Chip8Core(uint8_t soundAllowed){//Default constructor
	this->soundAllowed = soundAllowed;
	this->soundThisCycle = false;
}

void Chip8Core::initialize(void){
	this->pc = 512; // Program counter starts at 0x200 -> The first 512 bytes are reserved for the interpreter
	this->stackPointer = 0;
	this->I = 0;// Direction register reset
	this->DT = 0;// Delay timer reset
	this->ST = 0;// Sound timer reset
	//Register cleaning:
	for(int i=0; i<16; i++){
		this->V[i] = 0;
	}
	//Stack cleaning:
	for(int i=0; i<16; i++){
		this->stack[i] = 0;
	}
	//Key status cleaning:
	for(int i=0; i<16; i++){
		this->keys[i] = 0;
	}
	//Memory cleaning:
	for(int i=0; i<4096; i++){
		this->memory[i] = 0;
	}
	//Fontset loading in memory:
	for(int i=0; i<80; i++){
		this->memory[i] = fontSet[i];
	}
	//Screen cleaning:
	for(int i=0; i<2048; i++){
		this->screen[i] = 0;
	}
	this->soundThisCycle = false;
	srand(time(0));
}

int Chip8Core::emulateCycle(void){
	//Fetch the opcode
	uint16_t opcode = memory[pc] << 8 | memory[pc+1];//Opcodes have 2 bytes -> Sift and OR to form 2 bytes for the full opcode (Alt: //int opcode = ((memory[pc]<<8) & 0x0000FF00) | memory[pc+1];) ALT:memory[pc] << 8 | (0x00FF & memory[pc+1]);
	switch (opcode & 0xF000){//Getting the root of the opcode: XXXXXXXXXXXXXXXX & 1111000000000000 (extracting...)
		case 0x0000: return op0_(opcode);// Clean the screen OR Returns from a subrutine
			break;
		case 0x1000: op1NNN(opcode);// Jumps to address NNN
			break;
		case 0x2000: op2NNN(opcode);// Calls a subroutine at NNN
			break;
		case 0x3000: op3XKK(opcode);// Skips the next instruction if VX == KK
			break;
		case 0x4000: op4XKK(opcode);// Skips the next instruction if VX != KK
			break;
		case 0x5000: op5XY0(opcode);// Skips the next instruction if VX == VY
			break;
		case 0x6000: op6XKK(opcode);// Sets the register VX = KK
			break;
		case 0x7000: op7XKK(opcode);// Sets the register VX = VX + KK
			break;
		case 0x8000: return op8XY_(opcode);// This is a large subgroup of possible operations over the registers VX and VY
			break;
		case 0x9000: op9XY0(opcode);// Skips the next instruction if VX != VY
			break;
		case 0xA000: opANNN(opcode);// Sets I to NNN
			break;
		case 0xB000: opBNNN(opcode);// 0xBNNN Jumps to V[0] + NNN
			break;
		case 0xC000: opCXKK(opcode);// 0xCXKK Sets VX to a random byte AND KK
			break;
		case 0xD000: opDXYN(opcode);// Draws a sprite on the screen starting at coordinate (VX, VY)
			break;
		case 0xE000: return opE000(opcode);// Jump to the next instruction if VX is equal/different to the pressed key
			break;
		case 0xF000: return opF000(opcode);//Several options
			break;
		default: return 0;
	}
	return 1;//Ok
}

int Chip8Core::loadApplication(const char* appPath){
	char buffer = '0';
	FILE* file = fopen(appPath, "r");
	if(!file) return 0;
	int memFocus = 512;//Initial offset in the memory
	while(memFocus<4096 && !feof(file)){
		if(fread(&buffer, sizeof(char), 1, file)){
			memory[memFocus] = buffer;
			memFocus++;
		}
	}
	fclose(file);
	return 1;
}
	
void Chip8Core::updateTimers(void){
	if(DT>0){//Delay timer
		DT = DT - 1;
	}
	if(ST>0){//Sound timer
		ST = ST - 1;
		if(soundAllowed && ST>0){//When ST's value is greater than zero Chip-8 will sound [COW, WIK]
			this->soundThisCycle = true;
		}
	}
}

bool Chip8Core::shouldBeep(void){
	bool output = this->soundThisCycle;
	if(output)
		this->soundThisCycle = false;//Do not maintain a beep for several cycles
	return output;
}

bool Chip8Core::shouldDraw(void){
	bool output = this->draw;
	if(output)
		this->draw = false;
	return output;
}

const bool* Chip8Core::getScreen(void){
	return this->screen;
}

void Chip8Core::setKey(int focusKey, bool valKey){
	this->keys[focusKey] = valKey;
}

//Private methods:

int Chip8Core::op0_(uint16_t opcode){//0x0NNN will be ignored as done in modern interpreters -> Calls RCA 1802 program at address NNN. Not necessary for most ROMs. [WIK]
	switch(opcode & 0x00FF){
		//The opcode is 0x00E0 -> Clean the screen
		case 0x00E0:
			for(int i = 0; i<2048; i++){//64x32=2048
				screen[i] = 0;
			}
			pc = pc + 2;//Moving the program counter (by 2...)
			break;
		//The opcode is 0x00EE -> Returns from subroutine ()
		case 0x00EE:stackPointer = stackPointer - 1;
			pc = stack[stackPointer];
			pc = pc + 2;//Moving the program counter to the next (as the current value should be the original jump)
			break;
		default: return 0;
	}
	return 1;//Ok
}

void Chip8Core::op1NNN(uint16_t opcode){//0x1NNN -> Jumps to address NNN
	pc = opcode & 0x0FFF;//Getting the 12 bits direction (FFF = 4095, the maximum direction value)
}

void Chip8Core::op2NNN(uint16_t opcode){//0x2NNN -> Calls a subroutine at NNN.
	stack[stackPointer] = pc;//Saving the calling direction
	stackPointer = stackPointer + 1;
	pc = opcode & 0x0FFF;//Getting the 12 bits direction (FFF = 4095, the maximum direction value)
}

void Chip8Core::op3XKK(uint16_t opcode){//0x3XKK Skips the next instruction if VX == KK
	if (V[(opcode & 0x0F00) >> 8]==(opcode & 0x00FF)){//Skips the next instruction
		pc += 4;//Skipping
	}else{
		pc += 2;//Not skipping... this is the common pc's increment
	}
}

void Chip8Core::op4XKK(uint16_t opcode){//0x4XKK Skips the next instruction if VX != KK
	if (V[(opcode & 0x0F00) >> 8]!=(opcode & 0x00FF)){//Skips the next instruction
		pc += 4;//Skipping
	}else{
		pc += 2;//Not skipping... this is the common pc's increment
	}
}

void Chip8Core::op5XY0(uint16_t opcode){//0x5XY0 Skips the next instruction if VX == VY
	if(V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]){
		pc += 4;//Skipping
	}else{
		pc += 2;//Not skipping... this is the common pc's increment
	}
}

void Chip8Core::op6XKK(uint16_t opcode){//0x6XKK Sets the register VX = KK
	V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
	pc += 2;//Common pc's increment
}

void Chip8Core::op7XKK(uint16_t opcode){//0x7XKK Sets the register VX = VX + KK
	V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
	//[JEF] seems to have a bug at this point because of saving if there is a carry or not in V[0xF] -> E.g.: Tetris won't work properly
	pc += 2;//Common pc's increment
}

int Chip8Core::op8XY_(uint16_t opcode){//Subgroup of possible operations over the registers VX and VY
	int x, y, result;//for internal operations
	switch (opcode & 0x000F){
		case 0x0000:// 0x8XY0 Sets VX to VY
			V[(opcode & 0x0F00)>>8] = V[(opcode & 0x00F0)>>4];
			break;
		case 0x0001: //0x8XY1 Sets VX to VX OR VY
			V[(opcode & 0x0F00)>>8] |= V[(opcode & 0x00F0)>>4];
			break;
		case 0x0002: //0x8XY2 Sets VX to VX AND VY
			V[(opcode & 0x0F00)>>8] &= V[(opcode & 0x00F0)>>4];
			break;
		case 0x0003: //0x8XY3 Sets VX to "VX XOR VY"
			V[(opcode & 0x0F00)>>8] ^= V[(opcode & 0x00F0)>>4];
			break;
		case 0x0004: //0x8XY4 Adds VY to VX. VF is set to 1 when there's a carry (>8 bits required) (and to 0 otherwise)
			x = (opcode & 0x0F00) >> 8;
			y = (opcode & 0x00F0) >> 4;
			result = V[y] + V[x];//VY + VX
			if(result>0xFF){//255 = 0xFF (8 bits). The V* registers are of 8 bits
				V[0xF] = 1; //Carry
			}else{
				V[0xF] = 0;//No carry
			}
			V[x] = result;
			break;
		case 0x0005: // 0x8XY5 VY is subtracted from VX. VF is set to 0 when there's a borrow (and to 1 otherwise)
			x = (opcode & 0x0F00) >> 8;
			y = (opcode & 0x00F0) >> 4;
			if(V[x] >= V[y]){//If VX > VY -> VF is set to 1 [COW] ([JEF] does >= !! Why? <I WILL DO >= too>)
				V[x] = (V[x] - V[y]);
				V[0xF] = 1;//NOT borrow
			}else{
				V[x] = (/*0x100+*/V[x] - V[y]);//([JEF] takes 0x100 as borrow)
				V[0xF] = 0;//Borrow
			}
			break;
		case 0x0006: // 0x8XY6 Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift
			x = (opcode & 0x0F00) >> 8;
			V[0xF] = V[x] & 0x0001;//Getting the least significant value of VX
			result = V[x] >> 1;//Shifting by one (equivalent to dividing by two)
			V[x] = result;
			break;
		case 0x0007: // 0x8XY7 Sets VX to VY minus VX. VF is set to 0 when there's a borrow (and to 1 otherwise)
			x = (opcode & 0x0F00) >> 8;
			y = (opcode & 0x00F0) >> 4;
			if(V[y] >= V[x]){//If VY > VX there is no a borrow [COW] ([JEF] does >= !! Why? <I WILL DO >= too>)
				V[0xF] = 1;//NOT borrow
				V[x] = (V[y] - V[x]);
			}else{
				V[0xF] = 0;//borrow
				V[x] = (/*0x100+*/V[y] - V[x]);//([JEF] takes 0x100 as borrow)
			}
			break;
		case 0x000E: // 0x8XYE Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift
			x = (opcode & 0x0F00) >> 8;
			V[0xF] = (V[x] >> 7) & 0x1;//(V[x] & 0x80);//0x80=200_10=10000000_2////Getting the value of the most significant bit before shifting
			V[x] = (V[x] << 1);//Left Shifting				
			break;
		default: return 0;
	}
	pc += 2;//Common pc's increment for all the existing options
	return 1;//Ok
}

void Chip8Core::op9XY0(uint16_t opcode){// 0x9XY0 Skips the next instruction if VX != VY
	if(V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4]){
		pc += 4;//Skipping
	}else{
		pc += 2;//Not skipping... this is the common pc's increment
	}
}

void Chip8Core::opANNN(uint16_t opcode){// 0xANNN Sets I to NNN
	I = (opcode & 0x0FFF);
	pc += 2;
}

void Chip8Core::opBNNN(uint16_t opcode){// 0xBNNN Jumps to V[0] + NNN
	pc = ((opcode & 0x0FFF) + V[0]) & 0x0FFF;//The program counter (PC) should be 16-bit [COW] -> (Limiting to 4095)
}

void Chip8Core::opCXKK(uint16_t opcode){// 0xCXKK Sets VX to a random byte AND KK
	V[(opcode & 0x0F00) >> 8] = (rand() % 256) & (opcode & 0x00FF);//[0,1]*255 & KK
	pc += 2;
}

void Chip8Core::opDXYN(uint16_t opcode){// Draws a sprite on the screen starting at coordinate (VX, VY)
	int x = V[(opcode & 0x0F00)>>8];//Remember: integers are of 32 bits
	int y = V[(opcode & 0x00F0)>>4];
	int n = (opcode & 0x000F);
	int pixels, fX, fY, buffer;
	this->draw = true;	
	V[0xF] = 0;//VF is set to 1 if any screen pixels are flipped from set to unset 
	for(int i = 0; i<n; i++){//The sprite has a height of N
		pixels = memory[I+i];//Each row of 8 pixels is read as bit-coded starting from memory location I (which will remain unaltered)
		for(int j=0; j<8; j++){//Reading the row of 8 pixels
			if((pixels & (0x80>>j))!=0){//Accessing to the j^th bit: 0x80 = 10000000. We will shift according to j to progressively perform the AND where needed
				fX = (x+j) /*% 64*/;//Module forces turning around the screen, but some games are not compatible with this (e.g. Blitz) 
				fY = (y + i) /*% 32*/;
				if(fX>63 || fY>31) continue;
				buffer = fX + fY*64;
				if(screen[buffer] == true){
					V[0xF] = 1;//Collision 
					screen[buffer] = false;
				}else{
					screen[buffer] = true;
				}				
			}//We don't mind when it is 0 as 0_XOR_0 = 0 and 0_XOR_1 = 1 -> It does not change the screen
		}
	}
	pc += 2;
}

int Chip8Core::opE000(uint16_t opcode){// Jump to the next instruction if VX is equal/different to the pressed key
	switch(opcode & 0x00FF){
		case 0x009E: // EX9E: Skips the next instruction if the key stored in VX is pressed
			if(keys[V[(opcode & 0x0F00) >> 8]]){//==true
				pc += 4;
			}else{
				pc += 2;
			}
			break;
		case 0x00A1: // EXA1: Skips the next instruction if the key stored in VX isn't pressed
			if(!keys[V[(opcode & 0x0F00) >> 8]]){//==false
				pc += 4;
			}else{
				pc += 2;
			}
			break;
		default: return 0;
	}
	return 1;//ok
}

int Chip8Core::opF000(uint16_t opcode){
	int x = 0;
	switch(opcode & 0x00FF){
		case 0x0007: // FX07: Sets VX to the value of the delay timer
			V[(opcode & 0x0F00) >> 8] = (DT & 0x00FF);
			pc += 2;
			break;
		case 0x000A:{ // FX0A: A key press is expected and finally stored in VX		
			bool keyPress = false;
			x = (opcode & 0x0F00) >> 8;
			for(int i = 0; i < 16; i++){
				if(keys[i]){
					V[x] = i;
					keyPress = true; break;//Exit the loop (assuming just a required key)
				}
			}
			if(!keyPress){//No pressed key, skip this cycle and re-try in the next cycle (the program counter won't be moved)						
				return 1;//ok but no key yet
			}//Otherwise, we will move the program counter normally:
			pc += 2;
			break;
		}
		case 0x0015: // FX15: Sets the delay timer to VX
			DT = V[(opcode & 0x0F00) >> 8];
			pc += 2;
			break;
		case 0x0018: // FX18: Sets the sound timer to VX
			ST = V[(opcode & 0x0F00) >> 8];
			pc += 2;
			break;
		case 0x001E:{ // FX1E: Adds VX to I
			int result = I + V[(opcode & 0x0F00) >> 8];
			if(result > 0x0000FFF){// VF is set to 1 when range overflow (I+VX>0xFFF) and to 0 otherwise (See Note 3 in Wikipedia)
				V[0xF] = 1;
			}else{
				V[0xF] = 0;
			}
			I = result & 0x00000FFF;//Forcing 16 bits considering the register length (12 bits would force a valid direction though)
			pc += 2;
			break;
		}
		case 0x0029: // FX29: Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font
			I = V[(opcode & 0x0F00) >> 8] * 0x0005;
			pc += 2;
			break;
		case 0x0033: // FX33: Stores the Binary-coded decimal representation of VX at the addresses I, I plus 1, and I plus 2
			x = (opcode & 0x0F00) >> 8;
			memory[I]     = (uint8_t) floor(V[x]/100) % 10;
			memory[I + 1] = (uint8_t) floor(V[x]/10) % 10;
			memory[I + 2] = (V[x] % 10);					
			pc += 2;
			break;
		case 0x0055:{ // FX55: Stores V0 to VX in memory starting at address I					
			x = ((opcode & 0x0F00) >> 8);
			for (int i = 0; i <=x ; i++){
				memory[I + i] = V[i];
			}// On the original interpreter, when the operation is done, I = I + X + 1. [WIK]
			//I = I + x + 1; //On current implementations, I is left unchanged. (No problems found when doing it though)
			pc += 2;
			break;
		}
		case 0x0065: // FX65: Fills V0 to VX with values from memory starting at address I
			x = ((opcode & 0x0F00) >> 8);
			for (int i = 0; i <= x; ++i){
				V[i] = (memory[I + i] & 0x00FF);//The V registers are of 8 bits			
			}// On the original interpreter, when the operation is done, I = I + X + 1: [WIK]
			//I = I + x + 1;//On current implementations, I is left unchanged.!!<WARNING. CHANGING THIS BREAKS BLINKY AND SYZYGY>
			pc += 2;
			break;
		default: return 0;
	}
	return 1; //ok
}
