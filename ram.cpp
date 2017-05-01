#include "ram.h"


ram::ram(arch *config)
{
	this->config = new arch(config);
	data = new byte[this->config->getTotalMemory()];
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
	byte byteString[4];
	unsigned value;

	for (unsigned i = 0; i < 4; ++i)
		byteString[i] = data[from + i];

	value = byteString[0] * 0xFF000000;
	value += byteString[1] * 0xFF0000;
	value += byteString[2] * 0xFF00;
	value += byteString[3] * 0xFF;

	return value;
}
