#include "cacheLine.h"


cacheLine::cacheLine(address baseAddress, unsigned cacheLineSize)
{
	this->baseAddress = baseAddress;
	this->cacheLineSize = cacheLineSize;
	data = new byte[this->cacheLineSize];
}

cacheLine::~cacheLine()
{
	delete data;
}

void cacheLine::get(byte * from)
{
	for (unsigned i = 0; i < cacheLineSize; ++i)
		data[i] = from[i];
}

void cacheLine::put(byte * to)
{
	for (unsigned i = 0; i < cacheLineSize; ++i)
		to[i] = data[i];
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
