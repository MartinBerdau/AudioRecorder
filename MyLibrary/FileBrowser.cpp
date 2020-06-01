#include "FileBrowser.h"
#include "Arduino.h"

#include <string>
#include <cmath>

FileBrowser::FileBrowser()
{
	m_fs = 44100;		//Sampling frequency
	m_bitDepth = 16;	//Bits per Sample
	m_nrOfChannels = 1;	//Nr of Channels
	m_bytesPerSecond = (m_fs * m_bitDepth * m_nrOfChannels)/8; //Bytes (1 Byte = 8 Bits) per Second
}

FileBrowser::~FileBrowser()
{
}

void FileBrowser::computeCurName(char* Filename, unsigned int flagUpDown)
{
	int Wav1 = int(Filename[6] - '0');
	int Wav2 = int(Filename[7] - '0');
	if (flagUpDown == 1)
	{
		Wav2 += 1;
		if (Wav2 >= 10)
		{
			Wav1 += 1;
			Wav2 = 0;
		}
		Filename[6] = char(Wav1 + '0');
		Filename[7] = char(Wav2 + '0');
	}
	if (flagUpDown == 0)
	{
		int Wav1 = int(Filename[6] - '0');
		int Wav2 = int(Filename[7] - '0');
		if (Wav1 >= 0 && Wav2 >= 0)
		{
			if (Wav1 == 0 && Wav2 == 1)
			{
				Wav1 = 0;
				Wav2 = 1;
			}
			else
			{
				Wav2 -= 1;
				if (Wav2 < 0)
				{
					Wav1 -= 1;
					Wav2 = 9;
				}
			}
			Filename[6] = char(Wav1 + '0');
			Filename[7] = char(Wav2 + '0');
		}
	}
}

void FileBrowser::computeFileSizeChar(char* Size, unsigned int len_ms)
{
	unsigned long long fileLen_bytes = ((len_ms/1000) * m_bytesPerSecond + 36.0);
	createSizeChar(Size, fileLen_bytes);
}

void FileBrowser::computeFileLenChar(char* Len, unsigned int len_ms)
{
	unsigned long len_s = (len_ms / 1000);
	createLenChar(Len, len_s);
}

void FileBrowser::computeAvailableMemory(char* Memory, unsigned long long SDsize_bytes, unsigned long long MemoryUsed_bytes, unsigned long long availableMemory, unsigned long availableTime)
{
	availableMemory = SDsize_bytes - MemoryUsed_bytes;
	availableTime = (availableMemory - 36) / m_bytesPerSecond;
	createMemoryChar(Memory, availableMemory, availableTime);
	Serial.println(availableTime);
}

void FileBrowser::createMemoryChar(char* Memory, unsigned long long availableMemory, unsigned long availableTime)
{
	char helperSize[8];
	char helperLen[9] = "00:00:00";

	createSizeChar(helperSize, availableMemory);
	createLenChar(helperLen, availableTime);

	for (auto kk = 0U; kk < strlen(helperSize); kk++)
	{
		Memory[kk] = helperSize[kk];
	}

	for (auto kk = 0U; kk <= strlen(helperLen); kk++)
	{
		Memory[kk+11] = helperLen[kk];
	}
	
}

void FileBrowser::createSizeChar(char* size_Char, unsigned long long size)
{
	double helper1 = 0.0;
	double helper2 = 0.0;
	double kiMiGiB = 1024.0;
	double Size_kB = size / kiMiGiB;
	double Size_MB = Size_kB / kiMiGiB;
	double Size_GB = Size_MB / kiMiGiB;
	if (Size_GB >= 1.0) {
		helper1 = Size_GB;
		helper2 = helper1;
		size_Char[6] = 'G';
		size_Char[7] = 'B';
	}
	if (Size_GB < 1.0 && Size_MB >= 1.0) {
		helper1 = Size_MB;
		helper2 = helper1;
		size_Char[6] = 'M';
		size_Char[7] = 'B';
	}
	if (Size_MB < 1.0 && Size_kB >= 1.0) {
		helper1 = Size_kB;
		helper2 = helper1;
		size_Char[6] = 'k';
		size_Char[7] = 'B';
	}

	if (helper1 >= 100.0) {
		size_Char[0] = int(helper1 / 100);
		size_Char[1] = int(helper1 / 10) % 10 + '0';
		size_Char[2] = int(helper1) % 100 + '0';
		size_Char[3] = { '.' };
		size_Char[4] = int((helper1 - int(helper1)) * 10) + '0';
	}
	if (helper1 >= 10.0 && helper1 < 100.0) {
		size_Char[0] = int(helper1 / 10) + '0';
		size_Char[1] = int(helper1) % 10 + '0';
		size_Char[2] = { '.' };
		size_Char[3] = int((helper1 - int(helper1)) * 10) % 10 + '0';
		size_Char[4] = int(((helper1 - int(helper1)) * 10) / 10) + '0';
	}
	else
	{
		size_Char[0] = int(helper1) % 10 + '0';
		helper2 = int((helper1 - int(helper1)) * 100) % 100;
		size_Char[1] = { '.' };
		size_Char[2] = int(((helper1 - int(helper1)) * 100) / 100) + '0';
		size_Char[3] = int(helper2 / 10) + '0';
		size_Char[4] = int((helper1 - int(helper1)) * 100) % 10 + '0';
	}
}

void FileBrowser::createLenChar(char* len_Char, unsigned long len)
{
	len_Char[0] = char(int(len / 3600 / 10) + '0');
	len_Char[1] = char(int(len / 3600 % 10) + '0');
	len_Char[3] = char(int(len / 60 / 10 % 6) + '0');
	len_Char[4] = char(int(len / 60 % 10) + '0');
	len_Char[6] = char(int(len / 10 % 6) + '0');
	len_Char[7] = char(int(len % 10) + '0');
}

