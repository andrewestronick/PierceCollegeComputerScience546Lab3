#ifndef cache_h
#define cache_h

#include "arch.h"
#include "cacheLine.h"

class cache
{
public:
	cache(arch *config);
	~cache();
	bool checkFree(address from);
	void push(cacheLine *line);
	bool checkCache(address from);
	cacheLine* pull(address from); // not done

private:
	arch *config;
	byte **cacheMemory;
	struct tag
	{
		address memoryAddress;
		bool dirty;
		byte age;
	};
	tag **tagArray;

	unsigned getTag(address from);
	unsigned findFree(address from);
	void makeYoungest(unsigned tag, unsigned bank);
	unsigned getBank(address from);
};

#endif