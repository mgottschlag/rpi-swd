
#ifndef SWD_HPP_INCLUDED
#define SWD_HPP_INCLUDED

#include <stdint.h>

class SWD {
public:
	SWD(unsigned int frequency = 50000);
	~SWD();

	/**
	 * Resets the microcontroller.
	 */
	void reset();
	/**
	 * Sends 50 "1"s and 8 "0"s to resynchronize with the microcontroller.
	 */
	void resync();

	uint32_t read(bool ap, int reg);
	void write(bool ap, int reg, uint32_t value);
private:
	void writeBit(int value);
	int readBit();
	void clock();

	int writeCommand(bool ap, int read, int reg);

	void error();

	unsigned int tdiv2;

	int errors;
};

#endif

