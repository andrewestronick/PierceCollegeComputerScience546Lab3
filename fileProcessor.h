#ifndef fileprocessor_h
#define fileprocessor_h

#include "arch.h"
#include<string>
#include<vector>
#include<fstream>
#include<sstream>

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

#endif