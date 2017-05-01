#ifndef arch_h
#define arch_h

// Types of masks
#define OFFSET 1
#define TAG 2
#define MEMORY 4

class arch
{
public:
	arch(unsigned cacheLineSize, address cacheSize, unsigned cacheAssociativity, address totalMemory);
	arch(arch *config);
	~arch();
	unsigned getCacheLineSize();
	address getCacheSize();
	unsigned getCacheAssociativity();
	address getTotalMemory();
	unsigned getBits(unsigned type);
	address getMask(unsigned type);

private:
	unsigned cacheLineSize;
	address cacheSize;
	unsigned cacheAssociativity;
	address totalMemory;

	unsigned convertToBits(unsigned value);
};

#endif