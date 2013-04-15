
#ifndef DEBUGPORT_HPP_INCLUDED
#define DEBUGPORT_HPP_INCLUDED

#include <stdint.h>

class SWD;

class DebugPort {
public:
	DebugPort(SWD *swd);
	~DebugPort();

	uint32_t getIdCode();

	uint32_t getStatus();

	void select(unsigned int apSel, unsigned int apBank);

	uint32_t readRB();
	uint32_t readAP(unsigned int apSel, unsigned int address);
	void writeAP(unsigned int apSel, unsigned int address, uint32_t value);
private:
	SWD *swd;
	unsigned int currentAP;
	unsigned int currentBank;
};

#endif

