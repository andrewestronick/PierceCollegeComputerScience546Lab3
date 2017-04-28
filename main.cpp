// Estronick, Andrew CS546 Section 3122 Date April 25th, 2017
// Third Laboratory Assignment – Cache Simulation

#include"global.h"
#include"cacheLine.h"
#include "arch.h"

int main(int argc, char *argv[])
{
	arch *config = new arch(16, 2048, 2, 32768);


	

	delete config;
	return 0;
}