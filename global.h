#ifndef global_h
#define global_h

#define addressLenght 32
typedef unsigned __int32 address;
typedef unsigned char byte;

struct instruction
{
	char rw;
	address addr;
	unsigned value;
};

#endif