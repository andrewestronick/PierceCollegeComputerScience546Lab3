#include "arch.h"


arch::arch(unsigned cacheLineSize, address cacheSize, unsigned cacheAssociativity, address totalMemory)
{
	this->cacheLineSize = cacheLineSize;
	this->cacheSize = cacheSize;
	this->cacheAssociativity = cacheAssociativity;
	this->totalMemory = totalMemory;
}

arch::arch(arch *config)
{
	this->cacheLineSize = config->getCacheLineSize();
	this->cacheSize = config->getCacheSize();
	this->cacheAssociativity = config->getCacheAssociativity();
	this->totalMemory = config->getTotalMemory();
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

unsigned arch::getBits(unsigned value)
{
	unsigned bits = 1;

	for (; ((1u << bits) < value); ++bits);

	return bits;
}

unsigned arch::getOffsetBits()
{
	return getBits(cacheLineSize);
}

unsigned arch::getTagBits()
{
	return getBits(totalMemory / cacheSize / cacheAssociativity);
}

address arch::stripOffsetMask()
{
	unsigned bits = getOffsetBits();
	address mask = 0x0;

	for (unsigned i = 0; i < (32-bits); ++i)
	{
		mask |= 0x1;
		mask <<= 1;
	}

	mask <<= bits;

	return mask;
}

address arch::tagMask()
{
	unsigned bits = getTagBits();
	address mask = 0x0;

	for (unsigned i = 0; i < bits; ++i)
	{
		mask |= 0x1;
		mask <<= 1;
	}

	mask <<= getOffsetBits();

	return mask;
}

address arch::memoryMask()
{
	unsigned bits = addressLenght - (getTagBits() + getOffsetBits());
	address mask = 0x0;

	for (unsigned i = 0; i < bits; ++i)
	{
		mask |= 0x1;
		mask <<= 1;
	}
	
	mask <<= addressLenght - bits;

	return mask;
}
