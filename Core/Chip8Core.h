/**
 * This class contains the virtual CPU of the Chip-8 environment
 * @author N.C. Cruz, University of Almeria, Spain (2017)
 * SPECIAL THANKS TO LAURENCE MULLER FOR HIS BLOG (http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/) [MUL]
 * SPECIAL THANKS TO JEFFREY BIAN (http://mochi8.weebly.com/) [JEF]
 * SPECIAL THANKS TO http://devernay.free.fr/hacks/chip8/C8TECH10.HTM [COW]
 * SPECIAL THANKS TO http://omokute.blogspot.com.es/2012/06/emulation-basics-write-your-own-chip-8.html [OMO]
 * SPECIAL THANKS TO https://es.wikipedia.org/wiki/CHIP-8 [WIK]
 * SPECIAL THANKS TO https://github.com/michaelarnauts/chip8-java/blob/master/Source/src/be/khleuven/arnautsmichael/chip8/Cpu.java GREAT JOB!
 * Thank you very much for sharing your tutorials and information! I had ever wondered how emulators work!
 */


#ifndef CHIP8_CORE
#define CHIP8_CORE

#include <cstdio>
#include <time.h>
#include <cstdlib>
#include <stdint.h>
#include <cmath>

class Chip8Core{
	public:
		Chip8Core(uint8_t soundAllowed);//Default constructor

		void initialize(void);

		int emulateCycle(void);

		int loadApplication(const char* appPath);
	
		void updateTimers(void);

		const bool* getScreen(void);

		void setKey(int focusKey, bool valKey);

		bool shouldBeep(void);
		bool shouldDraw(void);
	private:
		bool soundThisCycle;
		bool draw;
		//Context:

		static uint8_t fontSet[80];//Fontset: sprites are used for graphics. Sprites are a set of bits that indicate a corresponding set of pixel's on/off state [OMO].

		uint8_t V[16];// Chip-8 has 16 8-bit registers: V0, V1, V2... VF
		
		uint16_t I;// Direction register (16 bits). Chip-8 has a 16-bit direction register
	
		uint16_t pc;// Program counter (16 bits). Hip-8 has a program counter which can have a value from 0x000 to 0xFFF (4095) (int data-type has been selected -> 32 bits)

		uint16_t stack[16];// Where to save the current location before jumping (up to 16 levels)

		uint8_t stackPointer;// Which level of the stack is currently used...

		bool screen[2048];// The Chip-8 system works in black and white with a screen of 2048 pixels (64 x 32)

		//Chip-8 has two special purpose 8-bit registers, for the delay and sound timers. When they are non-zero, they are automatically decremented at a rate of 60Hz. [COW]
		uint8_t DT;// Delay timer
		uint8_t ST;// Sound timer (it buzzes when ST==0)

		uint8_t memory[4096];//Main memory: The Chip 8 has 4KB of RAM memory in total [MUL]
		//Memory map [COW]:
		//0x000 (0) Start of Chip-8 RAM (0x000 to 0x1FF Reserved for the interpreter)
		//0x200 (512_(10)) Start of most Chip-8 programs
		//0xFFF (4095) End of Chip-8 RAM

		bool keys[16];// Chip 8 has a HEX based keypad (0x0-0xF), where to save their status-> press and release

		uint8_t soundAllowed; // Do you want to allow sound?

		//Opcode's implementation:
		
		int op0_(uint16_t opcode);//0x0NNN will be ignored as done in modern interpreters -> Calls RCA 1802 program at address NNN. Not necessary for most ROMs. [WIK]

		void op1NNN(uint16_t opcode);//0x1NNN -> Jumps to address NNN

		void op2NNN(uint16_t opcode);//0x2NNN -> Calls a subroutine at NNN.

		void op3XKK(uint16_t opcode);//0x3XKK Skips the next instruction if VX == KK

		void op4XKK(uint16_t opcode);//0x4XKK Skips the next instruction if VX != KK

		void op5XY0(uint16_t opcode);//0x5XY0 Skips the next instruction if VX == VY

		void op6XKK(uint16_t opcode);//0x6XKK Sets the register VX = KK

		void op7XKK(uint16_t opcode);//0x7XKK Sets the register VX = VX + KK

		int op8XY_(uint16_t opcode);//Subgroup of possible operations over the registers VX and VY

		void op9XY0(uint16_t opcode);// 0x9XY0 Skips the next instruction if VX != VY

		void opANNN(uint16_t opcode);// 0xANNN Sets I to NNN

		void opBNNN(uint16_t opcode);// 0xBNNN Jumps to V[0] + NNN

		void opCXKK(uint16_t opcode);// 0xCXKK Sets VX to a random byte AND KK

		void opDXYN(uint16_t opcode);// Draws a sprite on the screen starting at coordinate (VX, VY)

		int opE000(uint16_t opcode);// Jump to the next instruction if VX is equal/different to the pressed key

		int opF000(uint16_t opcode);
};

#endif
