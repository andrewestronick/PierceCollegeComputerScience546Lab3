#include "ram.h"


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
