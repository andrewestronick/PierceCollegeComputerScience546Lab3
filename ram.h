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
	void put(cacheLine *line);
	unsigned get32Value(address from);

private:
	arch *config;
	byte *data;
};

#endif
