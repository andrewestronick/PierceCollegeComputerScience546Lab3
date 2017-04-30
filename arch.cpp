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

arch::~arch() {}

unsigned arch::getCacheLineSize() { return cacheLineSize; }

address arch::getCacheSize() { return cacheSize; }

unsigned arch::getCacheAssociativity() { return cacheAssociativity; }

address arch::getTotalMemory() { return totalMemory; }

unsigned arch::getBits(unsigned type)
{
	switch (type)
	{
	case OFFSET:
		return convertToBits(cacheLineSize);

	case TAG:
		return convertToBits(totalMemory / cacheSize / cacheAssociativity);

	case MEMORY:
		return ( addressLenght - getBits(OFFSET) - getBits(TAG) );

	default:
		throw "Invalid type for getBits";
	}
}

address arch::getMask(unsigned type)
{
	unsigned mask = 0x0;
	unsigned bits;
	unsigned offset;

	switch (type)
	{
	case OFFSET:
		bits = getBits(OFFSET);
		offset = 0;
		break;

	case TAG:
		bits = getBits(TAG);
		offset = getBits(OFFSET);
		break;
	case MEMORY:
		bits = getBits(MEMORY);
		offset = getBits(TAG) + getBits(OFFSET);
		break;
	default:
		throw "Invalid type for getMask";
	}

	for (unsigned i = 0; i < (bits); ++i)
	{
		mask <<= 1;
		mask |= 0x1;
	}

	mask <<= offset;

	return mask;
}

unsigned arch::convertToBits(unsigned value)
{
	unsigned bits = 1;

	for (; ((1u << bits) < value); ++bits);

	return bits;
}
