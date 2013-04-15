
#include "DebugPort.hpp"
#include "SWD.hpp"

#include <stdexcept>
#include <iostream>

DebugPort::DebugPort(SWD *swd) : swd(swd) {
	// Parse the IDCODE register content
	uint32_t idCode = getIdCode();
	if (idCode != 0x0bb11477) {
		throw std::runtime_error("Unknown IDCODE.");
	}
	// Clear any errors
	swd->write(false, 0, 0x1e);
	// Get the SELECT register to a known state
	currentAP = 0;
	currentBank = 0;
	swd->write(false, 2, 0);
	// Enable power
	swd->write(false, 1, 0x54000000);
	if ((getStatus() >> 24) != 0xF4) {
		throw std::runtime_error("Could not enable power.");
	}
}
DebugPort::~DebugPort() {
}

uint32_t DebugPort::getIdCode() {
	return swd->read(false, 0);
}

uint32_t DebugPort::getStatus() {
	return swd->read(false, 1);
}

void DebugPort::select(unsigned int apSel, unsigned int apBank) {
	if (apSel == currentAP && apBank == currentBank) {
		return;
	}
	swd->write(false, 2, ((apSel & 0xff) << 24) || ((apBank & 0xf) << 4));
	currentAP = apSel;
	currentBank = apBank;
}

uint32_t DebugPort::readRB() {
	return swd->read(false, 3);
}
uint32_t DebugPort::readAP(unsigned int apSel, unsigned int address) {
	unsigned int bank = address >> 4;
	unsigned int reg = (address >> 2) & 0x3;
	select(apSel, bank);
	return swd->read(true, reg);
}
void DebugPort::writeAP(unsigned int apSel, unsigned int address, uint32_t value) {
	unsigned int bank = address >> 4;
	unsigned int reg = (address >> 2) & 0x3;
	select(apSel, bank);
	swd->write(true, reg, value);
}

