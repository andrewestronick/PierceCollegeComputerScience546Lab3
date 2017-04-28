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
