#include "fileProcessor.h"


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
	if(inFile.is_open())
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

