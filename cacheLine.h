#ifndef cacheline_h
#define cacheline_h

#include"global.h"
#include "arch.h"

class cacheLine
{
public:
	cacheLine(address baseAddress, unsigned cacheLineSize);
	~cacheLine();
	void get(byte *from);
	void put(byte *to);
	byte getByte(unsigned index);
	void putByte(unsigned index, byte value);
	address getBaseAddress();

private:
	address baseAddress;
	byte *data;
	unsigned cacheLineSize;
};

#endif