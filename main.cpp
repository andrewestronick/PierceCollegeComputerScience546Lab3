// Estronick, Andrew CS546 Section 3122 Date April 25th, 2017
// Third Laboratory Assignment – Cache Simulation

#include<iostream>
#include"global.h"
#include"cacheLine.h"
#include "arch.h"
#include "cache.h"
#include "fileProcessor.h"
#include "ram.h"

int main(int argc, char *argv[])
{
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