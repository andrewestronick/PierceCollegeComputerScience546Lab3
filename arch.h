#ifndef arch_h
#define arch_h

#include "global.h"

class arch
{
public:
	arch(unsigned cacheLineSize, address cacheSize, unsigned cacheAssociativity, address totalMemory);
	arch(arch *config);
	~arch();
	unsigned getCacheLineSize();
	address getCacheSize();
	unsigned getCacheAssociativity();
	address getTotalMemory();
	unsigned getBits(unsigned value);
	unsigned getOffsetBits();
	unsigned getTagBits();
	address stripOffsetMask();
	address tagMask();
	address memoryMask();


private:

	unsigned cacheLineSize;
	address cacheSize;
	unsigned cacheAssociativity;
	address totalMemory;
};

#endif