#include "cacheLine.h"


cacheLine::cacheLine(arch *config, address baseAddress)
{
	this->config = new arch(config);
	this->start = baseAddress;
	data = new byte[this->config->getCacheLineSize()];
}

cacheLine::~cacheLine()
{
	delete data;
	delete config;
}

void cacheLine::get(byte *from)
{
	for (unsigned i = 0, size = config->getCacheLineSize(); i < size; ++i)
		data[i] = from[start + i];
}

void cacheLine::put(byte *to, bool relative)
{
	address rstart = start & config->tagMask();

	if (relative)
		for (unsigned i = 0, size = config->getCacheLineSize(); i < size; ++i)
			to[rstart + i] = data[i];
	else
		for (unsigned i = 0, size = config->getCacheLineSize(); i < size; ++i)
			to[start + i] = data[i];
}

address cacheLine::getAddress()
{
	return start;
}

void cacheLine::setAddress(address start)
{
	this->start = start;
}
