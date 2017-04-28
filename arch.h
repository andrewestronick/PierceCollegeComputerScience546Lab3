#ifndef arch_h
#define arch_h

#include "global.h"

class arch
{
public:
	arch(unsigned cacheLineSize, address cacheSize, unsigned cacheAssociativity, address totalMemory);
	~arch();
	unsigned getCacheLineSize();
	address getCacheSize();
	unsigned getCacheAssociativity();
	address getTotalMemory();
	unsigned getGacheLineMask();

private:

	unsigned cacheLineSize;
	address cacheSize;
	unsigned cacheAssociativity;
	address totalMemory;
	unsigned cacheLineMask;
};

#endif