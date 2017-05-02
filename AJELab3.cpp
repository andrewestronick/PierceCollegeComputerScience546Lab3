// Estronick, Andrew CS546 Section 3122 Date April 25th, 2017
// Third Laboratory Assignment – Cache Simulation

#include<iostream>
#include<string>
#include<vector>
#include<fstream>
#include<sstream>

#define addressLenght 32
#define OFFSET 1
#define TAG 2
#define MEMORY 4
typedef unsigned __int32 address;
typedef unsigned char byte;

struct instruction
{
	char rw;
	address addr;
	unsigned value;
};

class arch
{
public:
	arch(unsigned cacheLineSize, address cacheSize, unsigned cacheAssociativity, address totalMemory);
	arch(arch *config);
	~arch();
	unsigned getCacheLineSize();
	address getCacheSize();
	unsigned getCacheAssociativity();
	address getTotalMemory();
	unsigned getBits(unsigned type);
	address getMask(unsigned type);

private:
	unsigned cacheLineSize;
	address cacheSize;
	unsigned cacheAssociativity;
	address totalMemory;

	unsigned convertToBits(unsigned value);
};

class cacheLine
{
public:
	cacheLine(arch *config, address baseAddress);
	~cacheLine();
	void get(byte *from, bool tagRelative);
	void put(byte *to, bool tagRelative);
	address getAddress();
	void setAddress(address start);

private:
	address start;
	byte *data;
	arch *config;
};

class fileProcessor
{
public:
	fileProcessor(std::string fileName);
	~fileProcessor();
	arch* getArch();
	std::vector<instruction>* getInstructions();

private:
	std::string fileName;
	std::ifstream inFile;
	unsigned cacheLineSize;
	address cacheSize;
	unsigned cacheAssociativity;
	address totalMemory;
	byte flag = 0;
	arch *config;
	instruction temp;
	std::vector<instruction> *input;
};

class ram
{
public:
	ram(arch *config);
	~ram();
	cacheLine* get(address from);
	void put(cacheLine *line);
	unsigned get32Value(address from);

private:
	arch *config;
	byte *data;
};

class cache
{
public:
	cache(arch *config);
	~cache();
	bool checkFree(address from);
	void push(cacheLine *line);
	bool checkCache(address from);
	cacheLine* pull(address from);
	cacheLine* flush(address from);
	void put32Value(address to, unsigned value);
	unsigned get32Value(address from);

private:
	arch *config;
	byte **cacheMemory;
	struct tag
	{
		address memoryAddress;
		bool dirty;
		unsigned age;
	};
	tag **tagArray;

	unsigned getTag(address from);
	unsigned findFree(address from);
	void makeYoungest(unsigned tag, unsigned bank);
	unsigned findOldest(address from);
	unsigned getBank(address from);
};

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		std::cout << "Usage: " << argv[0] << " {input file}\n";
		exit(-1);
	}

	fileProcessor *file;

	try
	{
		file = new fileProcessor(argv[1]);
	}
	catch (std::string e)
	{
		std::cout << e << std::endl;
		exit(-1);
	}

	arch *config = file->getArch();
	std::vector<instruction> *instructions = file->getInstructions();

	ram *memory = new ram(config);
	cache *systemCache = new cache(config);
	cacheLine *line;

	for (unsigned k = 0; k < instructions->size(); ++k)
	{
		char rw = instructions->at(k).rw;
		address addr = instructions->at(k).addr;
		address value = instructions->at(k).value;

		// std::cout << "k=" << k << "\trw=" << rw << "\taddr=" << addr << "\tvalue=" << value << std::endl;

		if (rw == 'R')
			if (!systemCache->checkCache(addr))
				printf("Address: %i\tmemory: %i\tcache: -1\n", addr, memory->get32Value(addr));
			else
			{
				unsigned cacheValue = systemCache->get32Value(addr);
				printf("Address: %i\tmemory: %i\tcache: %i\n", addr, memory->get32Value(addr), cacheValue);
			}
		else
		{
			if (!systemCache->checkCache(addr))
			{
				if (!systemCache->checkFree(addr))
				{
					line = systemCache->flush(addr);
					memory->put(line);
				}

				line = memory->get(addr);
				systemCache->push(line);

			}

			systemCache->put32Value(addr, value);
		}
	}

	delete file;
	delete config;
	delete instructions;
	delete memory;
	delete systemCache;
	return 0;
}

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
		return (addressLenght - getBits(OFFSET) - getBits(TAG));

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

void cacheLine::get(byte *from, bool tagRelative)
{
	address target = ((tagRelative) ? (start & config->getMask(TAG)) : (start));

	for (unsigned i = 0, size = config->getCacheLineSize(); i < size; ++i)
		data[i] = from[start + i];
}

void cacheLine::put(byte *to, bool tagRelative)
{
	address target = ((tagRelative) ? (start & config->getMask(TAG)) : (start));

	for (unsigned i = 0, size = config->getCacheLineSize(); i < size; ++i)
		to[target + i] = data[i];
}

address cacheLine::getAddress() { return start; }

void cacheLine::setAddress(address start) { this->start = start; }

fileProcessor::fileProcessor(std::string fileName)
{
	this->fileName = fileName;
	inFile.open(fileName);

	if (!inFile.is_open())
		throw std::string("Cannot open input file " + fileName);

	input = new std::vector<instruction>;

	for (std::string line; getline(inFile, line); )
	{
		if (line.size() == 0)
			continue;

		std::stringstream buffer(line);
		char command, arg2;
		int arg1, arg3;

		buffer >> command >> arg1;
		command = toupper(command);

		if (buffer.fail() || (command < 'A' && command > 'F'))
			throw std::string("invalid line: " + line);

		if (command < 'E')
		{
			if (arg1 <= 0 || (arg1 % 2) != 0)
				throw std::string("invalid value: " + line);

			switch (line[0])
			{

			case 'A':

				if (flag & 1)
					throw std::string("duplicate cache line size: " + line);

				flag |= 1;
				cacheLineSize = arg1;
				break;


			case 'B':

				if (flag & 2)
					throw std::string("duplicate cache size: " + line);

				flag |= 2;
				cacheSize = arg1;
				break;


			case 'C':

				if (flag & 4)
					throw std::string("duplicate associativity of the cache value : " + line);

				flag |= 4;
				cacheAssociativity = arg1;
				break;


			case 'D':

				if (flag & 8)
					throw std::string("duplicate size if memory : " + line);

				flag |= 8;
				totalMemory = arg1;
				break;


			default:
				;
			}
			continue;
		}

		if (command == 'F')
		{
			if (arg1 < 0 || (arg1 % 2) != 0)
				throw std::string("invalid value: " + line);

			temp.rw = 'R';
			temp.addr = arg1;
			temp.value = 0;
			input->emplace_back(temp);

			continue;
		}

		buffer >> arg2 >> arg3;
		arg2 = toupper(arg2);

		if (arg2 != 'W')
			throw std::string("invalid line: " + line);

		if ((arg1 < 0 || (arg1 % 2) != 0) || arg3 < 0)
			throw std::string("invalid value: " + line);

		temp.rw = 'W';
		temp.addr = arg1;
		temp.value = arg3;
		input->emplace_back(temp);
	}


	if (flag != 15)
		throw std::string("Error!!!  missing parameter");


	if (cacheLineSize > (0.1 * cacheSize))
		throw std::string("Line size may not exceed ten percent of the cache size.");

	if (cacheSize > (0.1 * totalMemory))
		throw std::string("Cache size may not exceed ten percent of the total size of main memory.");
}

fileProcessor::~fileProcessor()
{
	if (inFile.is_open())
		inFile.close();

	delete input;
}

arch * fileProcessor::getArch()
{
	arch *config = new arch(cacheLineSize, cacheSize, cacheAssociativity, totalMemory);
	return config;
}

std::vector<instruction>* fileProcessor::getInstructions()
{
	std::vector<instruction> *copy = new std::vector<instruction>;

	for (unsigned i = 0; i < input->size(); ++i)
	{
		copy->emplace_back(input->at(i));
	}

	return copy;
}


ram::ram(arch *config)
{
	this->config = new arch(config);
	address totalMemory = this->config->getTotalMemory();

	data = new byte[totalMemory];

	for (unsigned i = 0; i < totalMemory; ++i)
		data[i] = 0x0;
}

ram::~ram()
{
	delete data;
	delete config;
}

cacheLine* ram::get(address from)
{
	from &= ~config->getMask(OFFSET);
	cacheLine *line = new cacheLine(config, from);
	line->get(data, false);
	return line;
}

void ram::put(cacheLine *line) { line->put(data, false); }

unsigned ram::get32Value(address from)
{
	unsigned value = 0x00;

	for (unsigned i = 0; i < 4; ++i)
	{
		value <<= 8;
		value |= data[from + i];
	}

	return value;
}


cache::cache(arch *config)
{
	this->config = new arch(config);

	cacheMemory = new byte*[this->config->getCacheAssociativity()];

	for (unsigned i = 0; i < this->config->getCacheAssociativity(); ++i)
		cacheMemory[i] = new byte[this->config->getCacheSize() / this->config->getCacheAssociativity()];

	tagArray = new tag*[this->config->getCacheAssociativity()];

	for (unsigned i = 0; i < this->config->getCacheAssociativity(); ++i)
		tagArray[i] = new tag[this->config->getCacheSize() / this->config->getCacheAssociativity() / this->config->getCacheLineSize()];

	for (unsigned i = 0; i < this->config->getCacheAssociativity(); ++i)
		for (unsigned j = 0, size = (this->config->getCacheSize() / this->config->getCacheAssociativity()); j < size; ++j)
			cacheMemory[i][j] = 0x00;


	for (unsigned i = 0; i < this->config->getCacheAssociativity(); ++i)
		for (unsigned j = 0, size = this->config->getCacheSize() / this->config->getCacheAssociativity() / this->config->getCacheLineSize(); j < size; ++j)
		{
			tagArray[i][j].memoryAddress = 0x00;
			tagArray[i][j].dirty = false;
			tagArray[i][j].age = i;
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
	bool free = false;

	for (unsigned i = 0, size = config->getCacheAssociativity(); i < size; ++i)
		if (tagArray[i][tag].dirty == false)
			free = true;

	return free;
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
	address memoryAddress = from & ~config->getMask(OFFSET);

	for (unsigned i = 0, size = config->getCacheAssociativity(); i < size; ++i)
		if (tagArray[i][tag].memoryAddress == memoryAddress && tagArray[i][tag].dirty == true)
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

cacheLine* cache::flush(address from)
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
	unsigned tag = getTag(to);
	unsigned bank = getBank(to);
	address start = (to & ~config->getMask(MEMORY));

	for (unsigned i = 0; i < 4; ++i)
		cacheMemory[bank][start + i] = (value >> ((3 - i) * 8)) & 0xFF;

	makeYoungest(tag, bank);
}

unsigned cache::get32Value(address from)
{
	unsigned value = 0x00;
	unsigned tag = getTag(from);
	unsigned bank = getBank(from);

	for (unsigned i = 0; i < 4; ++i)
	{
		value <<= 8;
		value |= cacheMemory[bank][(from & ~config->getMask(MEMORY)) + i];
	}

	return value;
}

unsigned cache::getTag(address from)
{
	return ((from & config->getMask(TAG)) >> config->getBits(OFFSET));
}

unsigned cache::findFree(address from)
{
	unsigned bank;
	unsigned tag = getTag(from);

	for (unsigned i = 0; i < this->config->getCacheAssociativity(); ++i)
		if (false == tagArray[i][tag].dirty)
			bank = i;

	return bank;
}

void cache::makeYoungest(unsigned tag, unsigned bank)
{
	unsigned currentAge = tagArray[bank][tag].age;
	tagArray[bank][tag].age = 0;

	for (unsigned i = 0; i < this->config->getCacheAssociativity(); ++i)
	{
		unsigned age = tagArray[i][tag].age;

		if (i != bank && age <= currentAge)
			++tagArray[i][tag].age;
	}
}

unsigned cache::findOldest(address from)
{
	unsigned tag = getTag(from);
	unsigned age = 0;
	unsigned bank = 0;

	for (unsigned i = 0, size = this->config->getCacheAssociativity(); i < size; ++i)
		if (tagArray[i][tag].age >= age && tagArray[i][tag].dirty == true)
			bank = i;

	return bank;
}

unsigned cache::getBank(address from)
{
	unsigned tag = getTag(from);
	address memoryAddress = (from & ~config->getMask(OFFSET));

	for (unsigned i = 0, size = config->getCacheAssociativity(); i < size; ++i)
		if (tagArray[i][tag].memoryAddress == memoryAddress && tagArray[i][tag].dirty == true)
			return i;
}
