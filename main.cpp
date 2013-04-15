
#include "SWD.hpp"
#include "DebugPort.hpp"
#include "STM32.hpp"
#include "MemoryAccessPort.hpp"

#include <iostream>
#include <iomanip>
#include <fstream>

int main(int argc, char **argv) {
	SWD swd(500000);
	DebugPort debugPort(&swd);
	STM32 stm32(&debugPort);

	uint32_t dpIdCode = debugPort.getIdCode();
	uint32_t apIdCode = stm32.getAHB()->getIdCode();
	uint32_t mcuIdCode = stm32.getAHB()->readWord(0x40015800);

	std::cout << std::hex << std::setfill('0');
	std::cout << "DP.IDCODE: ";
	std::cout << std::setw(8) << dpIdCode << std::endl;
	std::cout << "AP.IDCODE: ";
	std::cout << std::setw(8) << apIdCode << std::endl;
	std::cout << "DBGMCU_IDCODE: ";
	std::cout << std::setw(8) << mcuIdCode << std::endl;

	if (argc == 1) {
		return 0;
	}

#if 1
	// Read the input file
	std::cout << "Reading program file..." << std::endl;
	char buffer[1 << 16];
	std::ifstream programFile(argv[1], std::ios_base::binary);
	if (!programFile) {
		std::cerr << "Could not open program file!" << std::endl;
		return -1;
	}
	programFile.read(buffer, 1 << 16);
	unsigned int size = programFile.gcount();
	size = (size + 3) & ~3;
	std::cout << "Program: " << std::dec << size << " bytes." << std::endl;
	programFile.close();
	
	std::cout << "Halting the processor..." << std::endl;
	stm32.halt();
	std::cout << "Unlocking the program memory..." << std::endl;
	stm32.unlockFlash();
	std::cout << "Erasing previous content..." << std::endl;
	for (unsigned int i = 0; i < ((size + 0x3ff) & ~0x3ff); i += 0x400) {
		std::cout << "Erasing " << std::hex << 0x08000000 + i << std::endl;
		stm32.eraseFlash(0x08000000 + i);
		for (unsigned int j = 0; j < 0x400; j += 4) {
			uint32_t erase = stm32.getAHB()->readWord(0x08000000 + i + j);
			if (erase != 0xffffffff) {
				std::cout << "Erasing failed at " << std::hex << 0x08000000 + i + j << std::endl;
			}
		}
	}
	std::cout << "Programming..." << std::endl;
	stm32.startProgramming();
	for (unsigned int i = 0; i < size; i += 4) {
		stm32.getAHB()->writeHalf(0x08000000 + i, *(uint32_t*)(buffer + i));
		stm32.waitFlash();
		uint32_t value = stm32.getAHB()->readWord(0x08000000 + i);
		if (value != *(uint32_t*)(buffer + i)) {
			std::cout << "Programming failed at " << std::hex << 0x08000000 + i << std::endl;
			std::cout << "Difference: " << *(uint32_t*)(buffer + i) << "/" << value << std::endl;
		}
	}
	//stm32.getAHB()->writeHalfs(0x08000000, size >> 2, (uint32_t*)buffer);
	stm32.endProgramming();
	std::cout << "Verifying memory content..." << std::endl;
	char *download = new char[size];
	//stm32.getAHB()->readBlock(0x08000000, size >> 2, (uint32_t*)download);
	for (unsigned int i = 0; i < size >> 2; i++) {
		((uint32_t*)download)[i] = stm32.getAHB()->readWord(0x08000000 + (i << 2));
	}
	for (unsigned int i = 0; i < size; i++) {
		if (buffer[i] != download[i]) {
			std::cout << "Error at byte " << std::dec << i << ": ";
			std::cout << std::hex << (unsigned int)buffer[i]
			          << "/" << (unsigned int)download[i] << std::endl;
		}
	}
	delete[] download;
	std::cout << "Resetting the processor..." << std::endl;
	stm32.unhalt();
	stm32.reset();
	std::cout << "Done." << std::endl;
#endif

#if 0
	uint32_t oldValue = stm32.getAHB()->readWord(0x20001000);
	stm32.getAHB()->writeWord(0x20001000, 0xdeadc0de);
	uint32_t newValue = stm32.getAHB()->readWord(0x20001000);

	std::cout << "Memory: " << std::hex << oldValue << "/" << newValue << std::endl;

	//uint32_t test[] = { 0x12345678, 0x5634129a, 0xabcdefab };
	//stm32.getAHB()->writeHalfs(0x20001004, 3, test);
	stm32.getAHB()->writeHalf(0x20001004, 0x12345678);
	stm32.getAHB()->writeHalf(0x20001008, 0x9abcdef0);
	newValue = stm32.getAHB()->readWord(0x20001004);
	std::cout << "Memory: " << std::hex << newValue << std::endl;
	newValue = stm32.getAHB()->readWord(0x20001008);
	std::cout << "Memory: " << std::hex << newValue << std::endl;
	newValue = stm32.getAHB()->readWord(0x2000100c);
	std::cout << "Memory: " << std::hex << newValue << std::endl;

	stm32.halt();
	oldValue = stm32.getAHB()->readWord(0x40022010);
	stm32.unlockFlash();
	newValue = stm32.getAHB()->readWord(0x40022010);
	std::cout << "Flash: " << std::hex << oldValue << "/" << newValue << std::endl;

	/*uint32_t test[] = { 0x12345678, 0x5634129a, 0xabcdefab };
	stm32.getAHB()->writeHalfs(0x08000000, 3, test);*/
	newValue = stm32.getAHB()->readWord(0x08000000);
	std::cout << "Memory: " << std::hex << newValue << std::endl;
	newValue = stm32.getAHB()->readWord(0x08000004);
	std::cout << "Memory: " << std::hex << newValue << std::endl;
	newValue = stm32.getAHB()->readWord(0x08000008);
	std::cout << "Memory: " << std::hex << newValue << std::endl;

	//stm32.eraseFlash(0x08000000);

	newValue = stm32.getAHB()->readWord(0x08000000);
	std::cout << "Memory: " << std::hex << newValue << std::endl;
	newValue = stm32.getAHB()->readWord(0x08000004);
	std::cout << "Memory: " << std::hex << newValue << std::endl;
	newValue = stm32.getAHB()->readWord(0x08000008);
	std::cout << "Memory: " << std::hex << newValue << std::endl;
#endif

	return 0;
}

