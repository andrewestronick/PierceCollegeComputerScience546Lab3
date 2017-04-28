#ifndef cacheline_h
#define cacheline_h

#include"global.h"
#include "arch.h"

class cacheLine
{
public:
	cacheLine(arch *config, address baseAddress);
	~cacheLine();
	byte getByte(unsigned index);
	void putByte(unsigned index, byte value);
	address getBaseAddress();

private:
	address baseAddress;
	byte *data;
	unsigned cacheLineSize;
};

#endif