#include "arch.h"


arch::arch(unsigned cacheLineSize, address cacheSize, unsigned cacheAssociativity, address totalMemory)
{
	this->cacheLineSize = cacheLineSize;
	this->cacheSize = cacheSize;
	this->cacheAssociativity = cacheAssociativity;
	this->totalMemory = totalMemory;
}

arch::~arch()
{
}

unsigned arch::getCacheLineSize()
{
	return cacheLineSize;
}

address arch::getCacheSize()
{
	return cacheSize;
}

unsigned arch::getCacheAssociativity()
{
	return cacheAssociativity;
}

address arch::getTotalMemory()
{
	return totalMemory;
}
