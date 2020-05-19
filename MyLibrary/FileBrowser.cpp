#include "FileBrowser.h"
#include "Arduino.h"


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

void FileBrowser::computeAvailableMemory(char* Memory, unsigned long long SDsize_bytes, unsigned long long MemoryUsed_bytes, unsigned long long availableMemory, unsigned long availableTime)
{
	availableMemory = SDsize_bytes - MemoryUsed_bytes;
	availableTime = (availableMemory - 36) / m_bytesPerSecond;
	createMemoryChar(Memory, availableMemory, availableTime);
	Serial.println(availableTime);
}

void FileBrowser::createMemoryChar(char* Memory, unsigned long long availableMemory, unsigned long availableTime)
{
	double helper1 = 0.0;
	double helper2 = 0.0;
	double kiMiGiB = 1024.0;
	double Memory_kB = availableMemory / kiMiGiB;
	double Memory_MB = Memory_kB / kiMiGiB;
	double Memory_GB = Memory_MB / kiMiGiB;
	if (Memory_GB >= 1.0) {
		helper1 = Memory_GB;
		helper2 = helper1;
		Memory[6] = 'G';
		Memory[7] = 'B';
	}
	if (Memory_GB < 1.0 && Memory_MB >= 1.0) {
		helper1 = Memory_MB;
		helper2 = helper1;
		Memory[6] = 'M';
		Memory[7] = 'B';
	}
	if (Memory_MB < 1.0 && Memory_kB >= 1.0) {
		helper1 = Memory_kB;
		helper2 = helper1;
		Memory[6] = 'k';
		Memory[7] = 'B';
	}

	if (helper1 >= 100.0) {
		Memory[0] = int(helper1 / 100);
		Memory[1] = int(helper1 / 10) % 10 + '0';
		Memory[2] = int(helper1) % 100 + '0';
		Memory[3] = { '.' };
		Memory[4] = int((helper1 - int(helper1)) * 10) + '0';
	}
	if (helper1 >= 10.0 && helper1 < 100.0) {
		Memory[0] = int(helper1 / 10) + '0';
		Memory[1] = int(helper1) % 10 + '0';
		Memory[2] = { '.' };
		Memory[3] = int((helper1 - int(helper1)) * 10) % 10 + '0';
		Memory[4] = int(((helper1 - int(helper1)) * 10) / 10) + '0';
	}
	else
	{
		Memory[0] = int(helper1) % 10 + '0';
		helper2 = int((helper1 - int(helper1)) * 100) % 100;
		Memory[1] = { '.' };
		Memory[2] = int(((helper1 - int(helper1)) * 100) / 100) + '0';
		Memory[3] = int(helper2 / 10) + '0';
		Memory[4] = int((helper1 - int(helper1)) * 100) % 10 + '0';
	}

	Memory[11] = char(int(availableTime / 3600 / 10) + '0');
	Memory[12] = char(int(availableTime / 3600 % 10) + '0');
	Memory[14] = char(int(availableTime / 60 / 10 % 6) + '0');
	Memory[15] = char(int(availableTime / 60 % 10) + '0');
	Memory[17] = char(int(availableTime / 10 % 6) + '0');
	Memory[18] = char(int(availableTime % 10) + '0');
}