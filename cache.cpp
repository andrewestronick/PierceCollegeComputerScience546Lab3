#include "cache.h"


cache::cache(arch *config)
{
	this->config = new arch(config);

	cacheMemory = new byte*[this->config->getCacheAssociativity()];

	for (unsigned i = 0; i < this->config->getCacheAssociativity(); ++i)
		cacheMemory[i] = new byte[this->config->getCacheSize() / this->config->getCacheAssociativity()];

	tagArray = new tag*[this->config->getCacheAssociativity()];

	for (unsigned i = 0; i < this->config->getCacheAssociativity(); ++i)
		tagArray[i] = new tag[this->config->getCacheSize() / this->config->getCacheAssociativity()];

	for (unsigned i = 0; i < this->config->getCacheAssociativity(); ++i)
		for (unsigned j = 0, size = (this->config->getCacheSize() / this->config->getCacheAssociativity()); j < size; ++j)
		{
			cacheMemory[i][j] = 0x0;
			tagArray[i][j].dirty = false;
			tagArray[i][j].age = j;
		}



}

cache::~cache()
{
	for (unsigned i = 0; i < this->config->getCacheAssociativity(); ++i)
		delete cacheMemory[i];
	
	delete cacheMemory;

	for (unsigned i = 0; i < this->config->getCacheAssociativity(); ++i)
		delete tagArray[i];

	delete tagArray;

	delete config;
}

bool cache::checkFree(address from)
{
	unsigned tag = getTag(from);
	
	for (unsigned i = 0; i < this->config->getCacheAssociativity(); ++i)
		if (tagArray[i][tag].dirty = false)
			return true;

	return false;
}

void cache::push(cacheLine *line)
{
	unsigned tag = getTag(line->getAddress());
	unsigned bank = findFree(line->getAddress());

	line->put(cacheMemory[bank], true);
	tagArray[bank][tag].dirty = true;
	tagArray[bank][tag].memoryAddress = line->getAddress();
	makeYoungest(tag, bank);

	delete line;
}

bool cache::checkCache(address from)
{
	unsigned tag = getTag(from);
	address memoryAddress = from & config->getMask(MEMORY);
	
	for (unsigned i = 0; i < config->getCacheAssociativity(); ++i)
		if (tagArray[i][tag].memoryAddress == memoryAddress)
			return true;

	return false;
}

cacheLine *cache::pull(address from)
{
	from &= ~config->getMask(OFFSET);
	unsigned tag = getTag(from);
	unsigned bank = getBank(from);

	cacheLine *line = new cacheLine(config, from);
	line->get(cacheMemory[bank], true);
	tagArray[bank][tag].dirty = false;
	makeYoungest(tag, bank);
	return line;
}

cacheLine * cache::flush(address from)
{
	from &= ~config->getMask(OFFSET);
	unsigned tag = getTag(from);
	unsigned bank = findOldest(from);
	address flushAddress = tagArray[bank][tag].memoryAddress;

	cacheLine *line = pull(flushAddress);

	return line;
}

void cache::put32Value(address to, unsigned value)
{
	byte byteString[4];

	byteString[0] = ((value & 0xFF000000) >> 24);
	byteString[1] = ((value & 0xFF0000) >> 16);
	byteString[2] = ((value & 0xFF00) >> 8);
	byteString[3] = (value & 0xFF);

	unsigned tag = getTag(to);
	unsigned bank = getBank(to);

	for (unsigned i = 0; i < 4; ++i)
		cacheMemory[bank][(to & ~config->getMask(MEMORY)) + i] = byteString[i];

	makeYoungest(tag, bank);
}

unsigned cache::get32Value(address from)
{
	byte byteString[4];
	unsigned value;

	unsigned tag = getTag(from);
	unsigned bank = getBank(from);

	for (unsigned i = 0; i < 4; ++i)
		byteString[i] = cacheMemory[bank][(from & ~config->getMask(MEMORY)) + i];


	value = byteString[0] * 0xFF000000;
	value += byteString[1] * 0xFF0000;
	value += byteString[2] * 0xFF00;
	value += byteString[3] * 0xFF;

	return value;
}

unsigned cache::getTag(address from)
{
	return ((from & config->getMask(TAG)) >> config->getBits(OFFSET));
}

unsigned cache::findFree(address from)
{
	unsigned bank;
	unsigned age = 0;

	for (unsigned i = 0; i < this->config->getCacheAssociativity(); ++i)
		if (false == tagArray[i][getTag(from)].dirty && tagArray[i][getTag(from)].age <= age)
		{
			bank = i;
			age = tagArray[i][getTag(from)].age;
		}

	return bank;
}

void cache::makeYoungest(unsigned tag, unsigned bank)
{
	unsigned currentAge = tagArray[bank][tag].age;
	tagArray[bank][tag].age = 0;

	for (unsigned i = 0; i < this->config->getCacheAssociativity(); ++i)
	{
		unsigned age = tagArray[i][tag].age;

		if ( i != bank && age <= currentAge)
			++tagArray[i][tag].age;
	}
}

unsigned cache::findOldest(address from)
{
	unsigned tag = getTag(from);
	unsigned age = 0;
	unsigned bank = 0;

	for (unsigned i = 0; i < this->config->getCacheAssociativity(); ++i)
		if (tagArray[i][tag].age >= age && tagArray[i][tag].dirty == true)
			bank = i;

	return bank;
}

unsigned cache::getBank(address from)
{
	unsigned tag = getTag(from);
	address memoryAddress = from & config->getMask(MEMORY);
	unsigned bank;

	for (unsigned i = 0; i < config->getCacheAssociativity(); ++i)
		if (tagArray[i][tag].memoryAddress == memoryAddress && tagArray[i][tag].dirty == true)
			bank = i;

	return bank;
}

