#ifndef ram_h
#define ram_h
#include "arch.h"

class ram
{
public:
	ram(arch *config);
	~ram();
private:
	address totalMemory;
	unsigned cacheLineSize;
	byte *data;
};

#endif
