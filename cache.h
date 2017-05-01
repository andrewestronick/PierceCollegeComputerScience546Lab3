#ifndef cache_h
#define cache_h

#include<iostream>
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
	cacheLine* pull(address from);
	cacheLine* flush(address from);
	void put32Value(address to, unsigned value);
	unsigned get32Value(address from);
	void status();

private:
	arch *config;
	byte **cacheMemory;
	struct tag
	{
		address memoryAddress;
		bool dirty;
		unsigned age;
	};
	tag **tagArray;

	unsigned getTag(address from);
	unsigned findFree(address from);
	void makeYoungest(unsigned tag, unsigned bank);
	unsigned findOldest(address from);
	unsigned getBank(address from);
};

#endif