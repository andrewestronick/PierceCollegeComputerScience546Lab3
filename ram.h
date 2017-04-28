#ifndef ram_h
#define ram_h
#include "global.h"
#include "arch.h"
#include "cacheLine.h"


class ram
{
public:
	ram(arch *config);
	~ram();
	cacheLine* get(address from);
	void put(address to, cacheLine *line);

private:
	address totalMemory;
	unsigned cacheLineSize;
	byte *data;
	unsigned cacheLineMask;
};

#endif
