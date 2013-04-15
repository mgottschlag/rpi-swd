
#include "STM32.hpp"
#include "MemoryAccessPort.hpp"

#include <unistd.h>

static const uint32_t FLASH_BASE = 0x40022000;

static const uint32_t FLASH_KEYR = FLASH_BASE + 0x04;
static const uint32_t FLASH_SR = FLASH_BASE + 0x0c;
static const uint32_t FLASH_CR = FLASH_BASE + 0x10;
static const uint32_t FLASH_AR = FLASH_BASE + 0x14;

STM32::STM32(DebugPort *debugPort) {
	ahb = new MemoryAccessPort(debugPort, 0);
}
STM32::~STM32() {
	delete ahb;
}

void STM32::halt() {
	ahb->writeWord(0xE000EDF0, 0xA05F0003);
}
void STM32::unhalt() {
	ahb->writeWord(0xE000EDF0, 0xA05F0000);
}
void STM32::reset() {
	ahb->writeWord(0xE000ED0C, 0x05FA0004);
}

void STM32::unlockFlash() {
	ahb->writeWord(FLASH_KEYR, 0x45670123);
	ahb->writeWord(FLASH_KEYR, 0xcdef89ab);
}
void STM32::lockFlash() {
	ahb->writeWord(FLASH_CR, ahb->readWord(FLASH_CR) | (1 << 7));
}

void STM32::startProgramming() {
	ahb->writeWord(FLASH_CR, 1);
}
void STM32::endProgramming() {
	ahb->writeWord(FLASH_CR, 0);
}

void STM32::eraseFlash(uint32_t page) {
	ahb->writeWord(FLASH_CR, 2);
	ahb->writeWord(FLASH_AR, page);
	ahb->writeWord(FLASH_CR, 0x42);
	while ((ahb->readWord(FLASH_SR) & 0x1) != 0) {
		usleep(10000);
	}
}

void STM32::eraseFlash() {
	// TODO
}

void STM32::waitFlash() {
	while ((ahb->readWord(FLASH_SR) & 0x1) != 0) {
		usleep(10000);
	}
}

