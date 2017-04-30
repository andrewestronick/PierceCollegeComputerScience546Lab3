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

}

bool cache::checkCache(address from)
{
	unsigned tag = getTag(from);
	address memoryAddress = from & config->memoryMask();
	
	for (unsigned i = 0; i < config->getCacheAssociativity(); ++i)
		if (tagArray[i][tag].memoryAddress == memoryAddress)
			return true;

	return false;
}

cacheLine *cache::pull(address from)
{
	unsigned bank = getBank(from);

	cacheLine *line = new cacheLine(config, (from & config->stripOffsetMask()));
	line->get(cacheMemory[bank]);
	// not finshed, need to clear cache cell
	return line;
}

unsigned cache::getTag(address from)
{
	return ((from & config->tagMask()) >> config->getOffsetBits());
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

unsigned cache::getBank(address from)
{
	unsigned tag = getTag(from);
	address memoryAddress = from & config->memoryMask();
	unsigned bank;

	for (unsigned i = 0; i < config->getCacheAssociativity(); ++i)
		if (tagArray[i][tag].memoryAddress == memoryAddress && tagArray[i][tag].dirty == true)
			bank = i;

	return bank;
}

