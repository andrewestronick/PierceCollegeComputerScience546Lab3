// Estronick, Andrew CS546 Section 3122 Date April 25th, 2017
// Third Laboratory Assignment – Cache Simulation

#include<iostream>
#include"global.h"
#include"cacheLine.h"
#include "arch.h"
#include "cache.h"

int main(int argc, char *argv[])
{
	arch *config = new arch(16, 2048, 2, 32768);


	
	// OFFSET TEST
	unsigned mask = config->getMask(OFFSET);

	std::cout << "OFFSET MASK " << mask << std::endl;

	for (unsigned i = 0; i < 32; ++i)
		std::cout << (((0x1 << (31 - i)) & mask) ? "1" : "0");

	std::cout << std::endl;



	// TAG TEST
	mask = config->getMask(TAG);

	std::cout << "TAG MASK " << mask << std::endl;

	for (unsigned i = 0; i < 32; ++i)
		std::cout << (((0x1 << (31 - i)) & mask) ? "1" : "0");

	std::cout << std::endl;



	// MEMORY TEST
	 mask = config->getMask(MEMORY);

	std::cout << "MEMORY MASK " << mask << std::endl;

	for (unsigned i = 0; i < 32; ++i)
		std::cout << (((0x1 << (31 - i)) & mask) ? "1" : "0");

	std::cout << std::endl;

	// cache *systemCache = new cache(config);

	// delete systemCache;
	delete config;
	return 0;
}