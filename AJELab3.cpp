// Estronick, Andrew CS546 Section 3122 Date April 25th, 2017
// Third Laboratory Assignment – Cache Simulation

#include<iostream>
#include<string>
#include<vector>
#include<map>
#include<fstream>
#include<sstream>

using namespace std;

#define addressLenght 32
typedef unsigned __int32 address;
typedef unsigned char byte;

struct instruction
{
	char rw;
	address addr;
	unsigned value;
};

class config
{
public:

	config::config(unsigned cacheLineSize, address cacheSize, unsigned cacheAssociativity, address totalMemory);
	string status();
	unsigned getTag(address addr);
	address getAssociativityAddress(address addr);
	unsigned getCacheLineSize();
	address getCacheSize();
	unsigned getCacheAssociativity();
	address getTotalMemory();
	unsigned getCacheLineOffsetForAddress(address addr);

private:

	unsigned cacheLineSize;				// Width of cache
	unsigned cacheLineOffsetBits;		// Number of bits needed to represent the cache line offset
	unsigned setsOfCacheLines;			// Number of sets of cache lines (groups) in cache
	unsigned lineSizeBits;				// Number of bits needed to represent the line size or tag
	address cacheSize;					// Total size of cache in bytes
	unsigned cacheAssociativity;		// Number of parallel cache lines
	unsigned asssociativityBits;		// Bits of main memory block that maps to a tag (left most bits)
	address totalMemory;				// Total main memory in bytes
	address cacheLineOffsetMask;		// Mask used to find cache line offset (right most bits)
	address cacheLineMask;				// Mask used to find cache line (middle bits)
	address asssociativityMask;			// Mask used to find asssociativity bits (left most bits)


	unsigned bitsForCacheLineOffset();	// Number of bits needed for the cache line offset
	unsigned groupsOfCacheLine();		// Return number of groups of cache lines
	unsigned bitsForCacheLine();		// Number of buts needed to represent the cache line
	unsigned bitsForAssociativity();	// Return bits for block of main memory that maps to a tag (left most bits)
	address getCacheLineOffsetMask();	// Get mask used to find cache line offset (right most bits)
	address getCacheLineMask();			// Get mask used to find cache line (middle bits)
	address getAsssociativityMask();	// Get mask used to find asssociativity bits (left most bits)

};


class ram
{
public:

	ram(config *configPtr);
	~ram();
	vector<byte> getCacheLine(address addr);
	void putCachline(address addr, vector<byte> cacheLine);
	byte getAddress(address addr);


private:

	address size;
	unsigned cacheAssociativity;
	unsigned cacheLineSize;
	byte *memory;

};


class cache
{
public:

	cache(config *configPtr);
	~cache();
	vector<byte> getCacheLine(address addr, unsigned group);
	void putCacheLine(address tag, unsigned group, vector<byte> cacheLine);
	void dumpMap();
	int checkCache(address addr);
	void promotoTagAge(unsigned tag, unsigned group);
	address getTagArrayValue(unsigned tagIndex, unsigned tagGroup, string type);
	void setTagArrayValue(unsigned tagIndex, unsigned tagGroup, string type, address addr);
	int findFreeGroup(address addr);
	void markGroupUsed(unsigned tagIndex, unsigned tagGroup);
	unsigned findOldestGroup(address addr);

private:

	unsigned cacheLineSize;
	address cacheSize;
	unsigned cacheAssociativity;
	map<string, byte> cacheMemory;
	map<string, address> tagArray;
	address cacheGroupSize;
	unsigned cacheLines;
	stringstream key;
	config *configPtr;
	byte** cacheBank;

};


void fileError(const string &errormsg, ifstream &file);


int main(int argc, char *argv[])
{
	// Check for correct command line usage
	if (argc != 2)
	{
		cout << "Usage: " << argv[0] << " {input file}\n";
		return -1;
	}

	unsigned lineSize;
	address cacheSize;
	unsigned assocCache;
	address totalMemory;
	vector<instruction> input;
	instruction temp;
	char flag = 0;

	// Open file and read contents into vector of strings
	ifstream inFile(argv[1]);

	if (!inFile.is_open())
	{
		cout << "Cannot open input file " << argv[1] << endl;
		return -1;
	}

	for (string line; getline(inFile, line); )
	{
		// Skip blank lines
		if (line.size() == 0)
			continue;

		stringstream buffer(line);
		char command, arg2;
		int arg1, arg3;


		buffer >> command >> arg1;
		command = toupper(command);

		if (buffer.fail() || (command < 'A' && command > 'F'))
			fileError("invalid line: " + line, inFile);

		if (command < 'E')
		{
			if (arg1 <= 0 || (arg1 % 2) != 0)
				fileError("invalid value: " + line, inFile);

			switch (line[0])
			{

			case 'A':

				if (flag & 1)
					fileError("duplicate line size: " + line, inFile);

				flag |= 1;
				lineSize = arg1;
				break;


			case 'B':

				if (flag & 2)
					fileError("duplicate cache size: " + line, inFile);

				flag |= 2;
				cacheSize = arg1;
				break;


			case 'C':

				if (flag & 4)
					fileError("duplicate associativity of the cache value : " + line, inFile);

				flag |= 4;
				assocCache = arg1;
				break;


			case 'D':

				if (flag & 8)
					fileError("duplicate size if memory : " + line, inFile);

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
				fileError("invalid value: " + line, inFile);
			
			temp.rw = 'R';
			temp.addr = arg1;
			temp.value = 0;
			input.emplace_back(temp);

			continue;
		}

		buffer >> arg2 >> arg3;
		arg2 = toupper(arg2);

		if (arg2 != 'W')
			fileError("invalid line: " + line, inFile);

		if ((arg1 < 0 || (arg1 % 2) != 0) || arg3 < 0)
			fileError("invalid value: " + line, inFile);
		
		temp.rw = 'W';
		temp.addr = arg1;
		temp.value = arg3;
		input.emplace_back(temp);
	}

	inFile.close();

	if (flag != 15)
	{
		cout << "Error!!!  missing parameter\n";
		return -1;
	}

	if (lineSize > (0.1 * cacheSize))
	{
		cout << "Line size may not exceed ten percent of the cache size.\n";
		return -1;
	}

	if (cacheSize > (0.1 * totalMemory))
	{
		cout << "Cache size may not exceed ten percent of the total size of main memory.\n";
		return -1;
	}


	config vm(lineSize, cacheSize, assocCache, totalMemory);

	ram memory(&vm);

	cache systemCache(&vm);

	for (unsigned k = 0; k < input.size(); ++k)
	{
		if ('W' == input[k].rw)
		{
			address addr = input[k].addr;

			unsigned value = input[k].value;

			vector<byte> valueBytes;

			valueBytes.push_back((value & 0xFF000000) >> 24);
			valueBytes.push_back((value & 0xFF0000) >> 16);
			valueBytes.push_back((value & 0xFF00) >> 8);
			valueBytes.push_back(value & 0xFF);

			int group = systemCache.checkCache(addr);

			if (-1 == group) // Not in Cache
			{
				int targetGroup = systemCache.findFreeGroup(addr);
				
				if (-1 != targetGroup) // Cache has room
				{
					vector<byte> cacheLine = memory.getCacheLine(addr);
					systemCache.putCacheLine(addr, targetGroup, cacheLine);
					systemCache.setTagArrayValue(vm.getTag(addr), targetGroup, "value", vm.getAssociativityAddress(addr));
					systemCache.promotoTagAge(vm.getTag(addr), targetGroup);
					systemCache.markGroupUsed(vm.getTag(addr), targetGroup);
					group = targetGroup;
				}
				else // Flush oldest cache group and replace
				{
					unsigned oldestGroup = systemCache.findOldestGroup(addr);
					vector<byte> cacheLine = systemCache.getCacheLine(addr, oldestGroup);
					address oldAddress = systemCache.getTagArrayValue(vm.getTag(addr),oldestGroup,"value");
					memory.putCachline(oldAddress, cacheLine);

					cacheLine = memory.getCacheLine(addr);
					systemCache.putCacheLine(addr, oldestGroup, cacheLine);
					systemCache.setTagArrayValue(vm.getTag(addr), targetGroup, "value", vm.getAssociativityAddress(addr));
					systemCache.promotoTagAge(vm.getTag(addr), targetGroup);
					systemCache.markGroupUsed(vm.getTag(addr), targetGroup);
					group = oldestGroup;
				}
			}

			vector<byte> cacheLine = systemCache.getCacheLine(addr, group);

			for (unsigned i = 0; i < valueBytes.size(); ++i)
				cacheLine[i + vm.getCacheLineOffsetForAddress(addr)] = valueBytes[i];

			systemCache.putCacheLine(addr, group, cacheLine);

			systemCache.setTagArrayValue(vm.getTag(addr), group, "value", vm.getAssociativityAddress(addr));
			systemCache.promotoTagAge(vm.getTag(addr), group);
			systemCache.markGroupUsed(vm.getTag(addr), group);
		}
		else // Command is F
		{
			address addr = input[k].addr;
			int group = systemCache.checkCache(addr);
			if (-1 == group)
			{
				printf("Address: %i\tmemory: %i\tcache: -1\n", addr, memory.getAddress(addr));
			}
			else
			{
				unsigned value = 0;

				vector<byte> cacheLine = systemCache.getCacheLine(addr, group);

				for (unsigned i = 0; i < 4; ++i)
				{
					value |= cacheLine[vm.getCacheLineOffsetForAddress(addr) + i];
					value <<= 8;
				}

				printf("Address: %i\tmemory: %i\tcache: %i\n", addr, memory.getAddress(addr), value);
			}
		}
	}
	
	return 0;
}


void fileError(const string &errormsg, ifstream &file)
{
	cout << "Error!!!  " << errormsg << endl;
	file.close();
	exit(-1);
}


config::config(unsigned cacheLineSize, address cacheSize, unsigned cacheAssociativity, address totalMemory)
{
	this->cacheLineSize = cacheLineSize;
	this->cacheSize = cacheSize;
	this->cacheAssociativity = cacheAssociativity;
	this->totalMemory = totalMemory;

	cacheLineOffsetBits = bitsForCacheLineOffset();
	setsOfCacheLines = groupsOfCacheLine();
	lineSizeBits = bitsForCacheLine();
	asssociativityBits = bitsForAssociativity();
	cacheLineOffsetMask = getCacheLineOffsetMask();
	cacheLineMask =  getCacheLineMask();
	asssociativityMask = getAsssociativityMask();
}


string config::status()
{
	stringstream status;

	status << "Cache line size = " << cacheLineSize << endl;
	status << "Number of bits needed to represent cache line offset = " << cacheLineOffsetBits << endl;
	status << "Size of cache = " << cacheSize << endl;
	status << "Associativity of cache (number of banks) = " << cacheAssociativity << endl;
	status << "Set or groups of cache lines = " << setsOfCacheLines << endl;
	status << "Number of bits needed to represent groups of cache lines = " << lineSizeBits << endl;
	status << "Size of total main memory = " << totalMemory << endl;
	status << "Number of bits needed to represent main memory block to tag mapping = " << asssociativityBits << endl;

	status << "\nBit pattern for address (A = Main memory block asssociativity, C = Cache line, O = Cache line offset).\n\n";
	for (int i = (addressLenght - 1); i >= 0; --i)
		status << ((i / 10) ? to_string(i / 10) : " ");
	status << endl;
	for (int i = (addressLenght - 1); i >= 0; --i)
		status << (i % 10);
	status << endl;
	for (unsigned i = 0; i < asssociativityBits; ++i)
		status << "A";
	for (unsigned i = 0; i < lineSizeBits; ++i)
		status << "C";
	for (unsigned i = 0; i < cacheLineOffsetBits; ++i)
		status << "O";
	status << endl;

	status << "Cache line offset mask = " << cacheLineOffsetMask << ".\n";
	status << "Cache line mask = " << cacheLineMask << ".\n";
	status << "Asssociativity mask = " << asssociativityMask << ".\n";

	return status.str();
}


unsigned config::getTag(address addr)
{
	return ((addr & cacheLineMask) >> cacheLineOffsetBits);
}


address config::getAssociativityAddress(address addr)
{
	return (addr & asssociativityMask);
}


unsigned config::getCacheLineSize()
{
	return cacheLineSize;
}


address config::getCacheSize()
{
	return cacheSize;
}


unsigned config::getCacheAssociativity()
{
	return cacheAssociativity;
}


address config::getTotalMemory()
{
	return totalMemory;
}

unsigned config::getCacheLineOffsetForAddress(address addr)
{
	return (addr & cacheLineOffsetMask);
}


unsigned config::bitsForCacheLineOffset()
{
	unsigned bits = 1;

	for (; ((1u << bits) < cacheLineSize); ++bits);

	return bits;
}


unsigned config::groupsOfCacheLine()
{
	return cacheSize / cacheLineSize / cacheAssociativity;
}


unsigned config::bitsForCacheLine()
{
	unsigned bits = 1;

	for (; ((1u << bits) < setsOfCacheLines); ++bits);

	return bits;
}


unsigned config::bitsForAssociativity()
{
	unsigned totalBits = addressLenght;
	return (totalBits - lineSizeBits - cacheLineOffsetBits);
}


address config::getCacheLineOffsetMask()
{
	address mask = 0;

	for(unsigned i = 0; i < cacheLineOffsetBits; ++i)
	{
		mask <<= 1;
		mask |= 1;

	}
	
	return mask;
}


address config::getCacheLineMask()
{
	address mask = 0;

	for (unsigned i = 0; i < lineSizeBits; ++i)
	{
		mask <<= 1;
		mask |= 1;
	}

	mask <<= cacheLineOffsetBits;

	return mask;
}


address config::getAsssociativityMask()
{
	address mask = 0;

	for (unsigned i = 0; i < asssociativityBits; ++i)
	{
		mask <<= 1;
		mask |= 1;
	}

	mask <<= (cacheLineOffsetBits + lineSizeBits);

	return mask;
}


ram::ram(config *configPtr)
{
	this->size = configPtr->getTotalMemory();
	this->cacheAssociativity = configPtr->getCacheAssociativity();
	this->cacheLineSize = configPtr->getCacheLineSize();

	memory = new byte[size];
	for (unsigned i = 0; i < size; ++i)
		memory[i] = 0x00;
}


ram::~ram()
{
	delete memory;
}


vector<byte> ram::getCacheLine(address addr)
{
	vector<byte> cacheLine;

	for (unsigned i = 0; i < cacheLineSize; ++i)
		cacheLine.push_back(memory[(addr + i)]);

	return cacheLine;
}


void ram::putCachline(address addr, vector<byte> cacheLine)
{
	for (unsigned i = 0; i < cacheAssociativity; ++i)
		memory[(addr + i)] = cacheLine[i];
}

byte ram::getAddress(address addr)
{
	return memory[addr];
}


cache::cache(config *configPtr)
{
	this->cacheLineSize = configPtr->getCacheLineSize();
	this->cacheSize = configPtr->getCacheSize();
	this->cacheAssociativity = configPtr->getCacheAssociativity();
	this->configPtr = configPtr;

	cacheGroupSize = cacheSize / cacheAssociativity;

	for(unsigned i = 0; i < cacheGroupSize; i+=cacheLineSize)
		for (unsigned j = 0; j < cacheAssociativity; ++j)
		{
			key.str("");
			key << i << "-" << j << "-";
			tagArray[key.str() + "value"] = 0x0;
			tagArray[key.str() + "used"] = 0x0;
			tagArray[key.str() + "dirty"] = 0x0;
			tagArray[key.str() + "age"] = j; // smaller is older
		}

	cacheBank = new byte*[cacheAssociativity];

	for (unsigned i = 0; i < cacheAssociativity; ++i)
	{
		cacheBank[i] = new byte[cacheGroupSize];
	}

	for (unsigned i = 0; i < cacheAssociativity; ++i)
		for (unsigned j = 0; j < cacheGroupSize; ++j)
			cacheBank[i][j] = 0x0;
}


cache::~cache()
{
	for (unsigned i = 0; i < cacheAssociativity; ++i)
		delete cacheBank[i];

	delete cacheBank;
}


vector<byte> cache::getCacheLine(address addr, unsigned group)
{
	vector<byte> cacheLine;
	address tag = configPtr->getTag(addr);

	for (unsigned i = 0; i < cacheLineSize; ++i)
		cacheLine.push_back(cacheBank[group][tag + i]);

	return cacheLine;
}


void cache::putCacheLine(address tag, unsigned group, vector<byte> cacheLine)
{
	for (unsigned i = 0; i < cacheLineSize; ++i)
		cacheBank[group][tag + i] = cacheLine[i];
}


void cache::dumpMap()
{
	for (const auto &p : tagArray)
	{
		std::cout << "tagArray[" << p.first << "] = " << p.second << '\n';
	}
}


int cache::checkCache(address addr)
{
	unsigned tagIndex = configPtr->getTag(addr);
	address tagValue = configPtr->getAssociativityAddress(addr);

	for (unsigned i = 0; i < cacheAssociativity; ++i)
		if (tagValue == getTagArrayValue(tagIndex, i, "value") && 1 == getTagArrayValue(tagIndex, i, "used"))
			return i;

	return -1; // We did not find cache line
}


void cache::promotoTagAge(unsigned tag, unsigned group)
{
	unsigned currentAge = getTagArrayValue(tag, group, "age");

	setTagArrayValue(tag, group, "age", 0);

	for (unsigned i = 0; i < cacheAssociativity; ++i)
	{
		address age = getTagArrayValue(tag, group, "age");

		if (i != group && age <= currentAge)
				setTagArrayValue(tag, group, "age", ++age);
	}

}


address cache::getTagArrayValue(unsigned tagIndex, unsigned tagGroup, string type)
{
	key.str("");
	key << tagIndex << "-" << tagGroup << "-" << type;

	return tagArray[key.str()];
}


void cache::setTagArrayValue(unsigned tagIndex, unsigned tagGroup, string type, address addr)
{
	key.str("");
	key << tagIndex << "-" << tagGroup << "-" << type;

	tagArray[key.str()] = addr;
}


int cache::findFreeGroup(address addr)
{
	unsigned tagIndex = configPtr->getTag(addr);

	for (unsigned i = 0; i < cacheAssociativity; ++i)
		if (getTagArrayValue(tagIndex, i, "used") == 0)
			return i;

	return -1;
}

void cache::markGroupUsed(unsigned tagIndex, unsigned tagGroup)
{
	setTagArrayValue(tagIndex, tagGroup, "used", 1);
}

unsigned cache::findOldestGroup(address addr)
{
	unsigned tagIndex = configPtr->getTag(addr);
	unsigned oldest = 0;

	for (unsigned i = 0; i < cacheAssociativity; ++i)
		if (getTagArrayValue(tagIndex, i, "age") < oldest && getTagArrayValue(tagIndex, i, "age") == 1)
			oldest = i;

	return oldest;
}
