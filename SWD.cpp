
#include "SWD.hpp"

#include <wiringPi.h>
#include <stdexcept>

/**
 * SWD clock pin (with pulldown in the microcontroller).
 */
static const int SWCLK = 12;
/**
 * SWD data pin (with pullup in the microcontroller).
 */
static const int SWDAT = 13;
/**
 * Microcontroller reset line (active low and with pullup in the
 * microcontroller).
 */
static const int NRST = 14;

static const int ACK_OK = 0x4;
static const int ACK_WAIT = 0x2;
static const int ACK_FAULT = 0x1;

#include <iostream>

SWD::SWD(unsigned int frequency) : errors(0) {
	tdiv2 = 1000000 / (frequency / 2);
	std::cout << "tdiv2: " << tdiv2 << std::endl;

	if (wiringPiSetup() == -1) {
		throw std::runtime_error("Could not initialize wiringPi.");
	}
	// We are doing realtime bitbanging - this might help a little bit
	piHiPri(99);

	pinMode(SWDAT, OUTPUT);
	digitalWrite(SWDAT, 0);
	pinMode(SWCLK, OUTPUT);
	digitalWrite(SWCLK, 0);

	reset();
	resync();
}
SWD::~SWD() {
}

void SWD::reset() {
	pinMode(NRST, OUTPUT);
	digitalWrite(NRST, 0);
	delay(200);
	pinMode(NRST, INPUT);
}
void SWD::resync() {
	for (unsigned int i = 0; i < 50; i++) {
		writeBit(1);
	}
	writeBit(0);
	// The reference says that we have to read IDCODE here
	read(false, 0);
}

uint32_t SWD::read(bool ap, int reg) {
	int ack = writeCommand(ap, 1, reg);
	if (ack == ACK_OK) {
		uint32_t value = 0;
		int parity = 0;
		for (unsigned int i = 0; i < 32; i++) {
			value >>= 1;
			int bit = readBit();
			value |= bit << 31;
			parity ^= bit;
		}
		if (parity == readBit()) {
			pinMode(SWDAT, OUTPUT);
			writeBit(0);
			errors = std::max(errors - 1, 0);
			return value;
		}
		std::cout << "value: " << std::hex << value << std::endl;
	} else if (ack == ACK_FAULT) {
		for (unsigned int i = 0; i < 33; i++) {
			clock();
		}
		pinMode(SWDAT, OUTPUT);
		writeBit(0);
		/*throw std::runtime_error("ACK_FAULT");*/
		std::cout << "ACK_FAULT" << std::endl;
		error();
		return read(ap, reg);
	} else if (ack == ACK_WAIT) {
		/*for (unsigned int i = 0; i < 33; i++) {
			clock();
		}*/
		// Just try again until the command succeeds
		/*pinMode(SWDAT, OUTPUT);
		return read(ap, reg);*/
		errors -= 2;
	}
	std::cout << "read ack: " << ack << std::endl;
	error();
	pinMode(SWDAT, OUTPUT);
	resync();
	return read(ap, reg);
}
void SWD::write(bool ap, int reg, uint32_t value) {
	int parity = __builtin_parity(value);
	int ack = writeCommand(ap, 0, reg);
	if (ack == ACK_OK) {
		clock();
		pinMode(SWDAT, OUTPUT);
		uint32_t tmpValue = value;
		for (unsigned int i = 0; i < 32; i++) {
			writeBit(tmpValue & 0x1);
			tmpValue >>= 1;
		}
		writeBit(parity);
		writeBit(0);
		errors = std::max(errors - 1, 0);
		return;
	} else if (ack == ACK_FAULT) {
		throw std::runtime_error("ACK_FAULT");
	} else if (ack == ACK_WAIT) {
		// Just try again until the command succeeds
		/*clock();
		pinMode(SWDAT, OUTPUT);
		for (unsigned int i = 0; i < 32; i++) {
			writeBit(0);
		}
		write(ap, reg, value);
		return;*/
		errors -= 2;
	}
	std::cout << "write ack: " << ack << std::endl;
	//throw std::runtime_error("test");
	error();
	pinMode(SWDAT, OUTPUT);
	resync();
	write(ap, reg, value);
}

void SWD::writeBit(int value) {
	digitalWrite(SWCLK, 1);
	delayMicroseconds(tdiv2);
	digitalWrite(SWDAT, value);
	digitalWrite(SWCLK, 0);
	delayMicroseconds(tdiv2);
}
int SWD::readBit() {
	digitalWrite(SWCLK, 1);
	delayMicroseconds(tdiv2);
	int value = digitalRead(SWDAT);
	digitalWrite(SWCLK, 0);
	delayMicroseconds(tdiv2);
	return value;
}
void SWD::clock() {
	digitalWrite(SWCLK, 1);
	delayMicroseconds(tdiv2);
	digitalWrite(SWCLK, 0);
	delayMicroseconds(tdiv2);
}

int SWD::writeCommand(bool ap, int read, int reg) {
	writeBit(1);
	writeBit(ap ? 1 : 0);
	writeBit(read);
	writeBit(reg & 0x1);
	writeBit((reg >> 1) & 0x1);
	int parity = (ap ? 1 : 0) + read + (reg & 0x1) + ((reg >> 1) & 0x1);
	writeBit(parity & 0x1);
	writeBit(0);
	pinMode(SWDAT, INPUT);
	/*if (reg == 1) {
		uint32_t data = 0;
		for (unsigned int i = 0; i < 32; i++) {
			data <<= 1;
			data |= readBit();
		}
		std::cout << std::hex << data << std::endl;
	}*/
	clock();
	clock();
	int ack = readBit() << 2;
	ack |= readBit() << 1;
	ack |= readBit();
	return ack;
}

void SWD::error() {
	errors += 2;
	if (errors >= 10) {
		throw std::runtime_error("Too many errors.");
	}
}

