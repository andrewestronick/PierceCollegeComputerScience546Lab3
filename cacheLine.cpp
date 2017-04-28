#include "cacheLine.h"


cacheLine::cacheLine(arch *config, address baseAddress)
{
	this->baseAddress = baseAddress;
	this->cacheLineSize = config->getCacheLineSize();
	data = new byte[this->cacheLineSize];
}

cacheLine::~cacheLine()
{
	delete data;
}

byte cacheLine::getByte(unsigned index)
{
	if (index < cacheLineSize)
		return data[index];

	throw "index out of range";
}

void cacheLine::putByte(unsigned index, byte value)
{
	if (index < cacheLineSize)
		data[index] = value;

	throw "index out of range";
}

address cacheLine::getBaseAddress()
{
	return baseAddress;
}
