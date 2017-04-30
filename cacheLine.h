#ifndef cacheline_h
#define cacheline_h

#include"global.h"
#include "arch.h"

class cacheLine
{
public:
	cacheLine(arch *config, address baseAddress);
	~cacheLine();
	void get(byte *from);
	void put(byte *to, bool relative);
	address getAddress();
	void setAddress(address start);

private:
	address start;
	byte *data;
	arch *config;
};

#endif