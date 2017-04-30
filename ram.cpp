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
	from &= config->stripOffsetMask();
	cacheLine *line = new cacheLine(config, from);
	line->get(data);
	return line;
}

void ram::put(cacheLine *line)
{
	line->put(data, false);
}
