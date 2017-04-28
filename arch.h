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

private:

	unsigned cacheLineSize;
	address cacheSize;
	unsigned cacheAssociativity;
	address totalMemory;
};

#endif