#include "ram.h"


ram::ram(arch *config)
{
	this->totalMemory = config->getTotalMemory();
	this->cacheLineSize = config->getCacheSize();
	data = new byte[this->totalMemory];
}

ram::~ram()
{
	delete data;
}

cacheLine* ram::get(address from)
{
	cacheLine *line = new cacheLine(from, cacheLineSize);
	line->get(data);
	return line;
}
