
#ifndef MEMORY_ACCESS_PORT_HPP_INCLUDED
#define MEMORY_ACCESS_PORT_HPP_INCLUDED

#include <stdint.h>

class DebugPort;

class MemoryAccessPort {
public:
	MemoryAccessPort(DebugPort *debugPort, unsigned int apSel);
	~MemoryAccessPort();

	uint32_t getIdCode();

	uint32_t readWord(uint32_t address);
	uint32_t writeWord(uint32_t address, uint32_t value);

	uint32_t readHalf(uint32_t address);
	uint32_t writeHalf(uint32_t address, uint32_t value);

	void readBlock(uint32_t address, uint32_t count, uint32_t *buffer);
	void writeBlock(uint32_t address, uint32_t count, const uint32_t *buffer);
	void writeBlockNonInc(uint32_t address,
	                      uint32_t count,
	                      const uint32_t *buffer);
	void writeHalfs(uint32_t address, uint32_t count, const uint32_t *buffer);
	void writeHalfsVerify(uint32_t address, uint32_t count, const uint32_t *buffer);
private:
	void csw(unsigned int addrInc, unsigned int size);

	DebugPort *debugPort;
	unsigned int apSel;
};

#endif

