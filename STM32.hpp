
#ifndef STM32_HPP_INCLUDED
#define STM32_HPP_INCLUDED

#include <stdint.h>

class DebugPort;
class MemoryAccessPort;

class STM32 {
public:
	STM32(DebugPort *debugPort);
	~STM32();

	void halt();
	void unhalt();
	void reset();

	void unlockFlash();
	void lockFlash();

	void startProgramming();
	void endProgramming();

	void eraseFlash(uint32_t page);
	void eraseFlash();

	void waitFlash();

	MemoryAccessPort *getAHB() {
		return ahb;
	}
private:
	MemoryAccessPort *ahb;
};

#endif

