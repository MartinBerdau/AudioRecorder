#pragma once


class FileBrowser
{
public:
	FileBrowser();
	~FileBrowser();

	void computeCurName(char* lastFilename, unsigned int flagUpDown);
	void computeAvailableMemory(char* Memory, unsigned long long SDsize_bytes, unsigned long long MemoryUsed_bytes, unsigned long long availableMemory, unsigned long availableTime);
	void createMemoryChar(char* Memory, unsigned long long availableMemory, unsigned long availableTime);

private:
	unsigned int m_fs;
	unsigned int m_bitDepth;
	unsigned int m_nrOfChannels;
	unsigned int m_bytesPerSecond;
};

