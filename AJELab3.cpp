// Estronick, Andrew CS546 Section 3122 Date April 25th, 2017
// Third Laboratory Assignment – Cache Simulation

#include<iostream>
#include<string>
#include<vector>
#include<fstream>
#include<sstream>

using namespace std;

void fileError(const string &errormsg, ifstream &file);

int main(int argc, char *argv[])
{
	// Check for correct command line usage
	if (argc != 2)
	{
		cout << "Usage: " << argv[0] << " {input file}\n";
		return -1;
	}

	int lineSize;
	int cacheSize;
	int assocCache;
	int totalMemory;
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

	cout << "Line size " << lineSize << endl;
	cout << "Cache size " << cacheSize << endl;
	cout << "Cache assoc. " << assocCache << endl;
	cout << "Total memory " << totalMemory << endl << endl;
	for (unsigned i = 0; i < input.size(); ++i)
		cout << input[i] << endl;

	return 0;
}

void fileError(const string &errormsg, ifstream &file)
{
	cout << "Error!!!  " << errormsg << endl;
	file.close();
	exit(-1);
}

