// Estronick, Andrew CS546 Section 3122 Date April 25th, 2017
// Third Laboratory Assignment � Cache Simulation

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

class config
{
public:

	config::config(unsigned cacheLineSize, address cacheSize, unsigned cacheAssociativity, address totalMemory);
	string status();
	unsigned getTag(address addr);

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

	ram(address size, unsigned cacheAssociativity);
	~ram();
	vector<byte> getCacheLine(address addr);
	void putCachline(address addr, vector<byte> cacheLine);


private:

	address size;
	unsigned cacheAssociativity;
	byte *memory;

};


class cache
{
public:

	cache(unsigned cacheLineSize, address cacheSize, unsigned cacheAssociativity);
	~cache();
	vector<byte> getCacheLine(address tag);
	void putCacheLine(address tag, vector<byte>);

private:

	unsigned cacheLineSize;
	address cacheSize;
	unsigned cacheAssociativity;
	map<string, byte> cacheMemory;
	map<string, address> tagArray;
	address cacheGroupSize;
	unsigned cacheLines;
	stringstream key;
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
	vector<string> input;
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

			input.emplace_back(string(1, command) + " " + to_string(arg1));

			continue;
		}

		buffer >> arg2 >> arg3;
		arg2 = toupper(arg2);

		if (arg2 != 'W')
			fileError("invalid line: " + line, inFile);

		if ((arg1 < 0 || (arg1 % 2) != 0) || arg3 < 0)
			fileError("invalid value: " + line, inFile);
		
		input.emplace_back(string(1, command) + " " + to_string(arg1) + " W " + to_string(arg3));
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

	ram memory(totalMemory, assocCache);
	cache systemCache(lineSize, cacheSize, assocCache);

	/*
	int cacheByteOffset = valueToPowerOf2Bits(lineSize);
	int cacheBlocks = cacheSize / lineSize;
	int blocks = cacheBlocks / assocCache;

	char *memory = new char[totalMemory];
	for (int i = 0; i < totalMemory; ++i)
		memory[i] = 0;

	char *cache = new char[cacheSize];
	for (int i = 0; i < cacheSize; ++cacheSize)
		cache[i] = 0;

	tag *tagArray = new tag[cacheBlocks];
	for (int i = 0; i < cacheBlocks; ++i)
	{
		tagArray[i].cacheLine = 0;
		tagArray[i].dirty = false;
	}

	int *ageQueue = new int[cacheBlocks];
	for (int i = 0; i < blocks; ++i)
		for (int j = 0; j < assocCache; ++j)
			ageQueue[(cacheBlocks * assocCache) + j] = j;
		
	for (int i = 0; i < input.size(); ++i)
	{
		stringstream line(input[i]);
		char command;

		line >> command;

		if ('E' == command)
		{

		}
		else
		{
			int address;
			line >> address;
			printf("Address: %i\tmemory: %i\tcache: %i\t\n",address, memory[address], cache[address]);
		}
	}
	
	delete memory;
	delete cache;
	delete tagArray;
	delete ageQueue;
	*/
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
	return (addr & cacheLineMask);
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

ram::ram(address size, unsigned cacheAssociativity)
{
	this->size = size;
	this->cacheAssociativity = cacheAssociativity;

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

	for (unsigned i = 0; i < cacheAssociativity; ++i)
		cacheLine.push_back(memory[(addr + i)]);

	return cacheLine;
}

void ram::putCachline(address addr, vector<byte> cacheLine)
{
	for (unsigned i = 0; i < cacheAssociativity; ++i)
		memory[(addr + i)] = cacheLine[i];
}

cache::cache(unsigned cacheLineSize, address cacheSize, unsigned cacheAssociativity)
{
	this->cacheLineSize = cacheLineSize;
	this->cacheSize = cacheSize;
	this->cacheAssociativity = cacheAssociativity;

	cacheGroupSize = cacheSize / cacheAssociativity;

	for(unsigned i = 0; i < cacheGroupSize; i+=cacheLineSize)
		for (unsigned j = 0; j < cacheAssociativity; ++j)
		{
			key.str("");
			key << i << " " << j;
			tagArray[key.str() + "value"] = 0x0;
			tagArray[key.str() + "used"] = 0x0;
			tagArray[key.str() + "dirty"] = 0x0;
			tagArray[key.str() + "age"] = (cacheAssociativity - j);
		}
}

cache::~cache()
{

}

vector<byte> cache::getCacheLine(address tag)
{
	vector<byte> cacheLine;



	return cacheLine;
}

void cache::putCacheLine(address tag, vector<byte>)
{

}
