/*
Copyright 2020 Tammo Sander

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR
A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file contains functions that are used in the file browser of this Project.
For example a function to compute the name of the file or the size of a file.
*/

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

//function to compute and generate the file name 
//(paramters: Filename = last file's name, flagUpDown = count up or down)
void FileBrowser::computeCurName(char* Filename, unsigned int flagUpDown)
{
	//get the count of the last file
	int Wav1 = int(Filename[6] - '0');
	int Wav2 = int(Filename[7] - '0');
	//if flagUpDow = 1, filename gets increased
	if (flagUpDown == 1)
	{
		Wav2 += 1;
		//check if second digit is over 9 and if the first digit has to be changed as well
		if (Wav2 >= 10)
		{
			Wav1 += 1;
			Wav2 = 0;
		}
		//set the updated digits in the char array
		Filename[6] = char(Wav1 + '0');
		Filename[7] = char(Wav2 + '0');
	}
	//if flagUpDown = 0, filename gets decreased
	if (flagUpDown == 0)
	{
		//check if file count can be decreased
		if (Wav1 >= 0 && Wav2 >= 0)
		{
			//check if first file is choosen
			if (Wav1 == 0 && Wav2 == 1)
			{
				Wav1 = 0;
				Wav2 = 1;
			}
			//else decrease
			else
			{
				Wav2 -= 1;
				//if second digit is smaller than 0, count down first digit as well
				if (Wav2 < 0)
				{
					Wav1 -= 1;
					Wav2 = 9;
				}
			}
			//set the updated digits in the char array
			Filename[6] = char(Wav1 + '0');
			Filename[7] = char(Wav2 + '0');
		}
	}
}

//function to compute and generate the displayable file size char array 
//Paramters: Size = the displayed char array, len_byte = length of the file in bytes
void FileBrowser::computeFileSizeChar(char* Size, unsigned long len_byte)
{
	//convert it to long long and call the createSizeChar-function to create the char array
	unsigned long long fileLen_bytes = len_byte;
	createSizeChar(Size, fileLen_bytes);
}

//function to compute and generate the displayable file length char array
//Parameters: Len = the displayed char array, len_byte = length of the file in bytes
void FileBrowser::computeFileLenChar(char* Len, unsigned long len_byte)
{
	//compute the length of the file in seconds and call the createLenChar-function to create the char array
	unsigned long len_s = len_byte / m_bytesPerSecond;
	createLenChar(Len, len_s);
}

//function to compute and generate the displayable available Memory (bytes and time) char array
//Parameters: Memory = char array thats displayed, SDsize_bytes = Size of the SD Card in bytes,
//				MemoryUsed_bytes = memory thats already used in bytes, availableMemory = free Memory in bytes
//				availableTime = available time in seconds
void FileBrowser::computeAvailableMemory(char* Memory, unsigned long long SDsize_bytes, unsigned long long MemoryUsed_bytes, unsigned long long availableMemory, unsigned long availableTime)
{
	//compute the free space on the SD Card in bytes
	availableMemory = SDsize_bytes - MemoryUsed_bytes;
	//compute the available time in seconds by using the available Memory - 36 bytes, which are necessary
	//to save the header of the wave file
	availableTime = (availableMemory - 36) / m_bytesPerSecond;
	//call createMemoryChar function to compute the full char array
	createMemoryChar(Memory, availableMemory, availableTime);
}

//function to create the available Memory char array
//Parameters: Memory = char array thats displayed, availableMemory = free Memory in bytes, 
// availableTime = available time on SD Card
void FileBrowser::createMemoryChar(char* Memory, unsigned long long availableMemory, unsigned long availableTime)
{
	//create two helper arrays
	char helperSize[8];
	char helperLen[9] = "00:00:00";

	//call the functions to generate the memory and time char array
	createSizeChar(helperSize, availableMemory);
	createLenChar(helperLen, availableTime);

	//copy the generated char arrays in the output char array
	for (auto kk = 0U; kk < strlen(helperSize); kk++)
	{
		Memory[kk] = helperSize[kk];
	}

	for (auto kk = 0U; kk <= strlen(helperLen); kk++)
	{
		Memory[kk+11] = helperLen[kk];
	}
	
}

//function to create the file size char array
//Parameters: size_char = size char array, size = size of file / size of available memory
void FileBrowser::createSizeChar(char* size_Char, unsigned long long size)
{
	double helper1 = 0.0;
	double helper2 = 0.0;
	double kiMiGiB = 1024.0; //helper to compute size with prefix
	double Size_kB = size / kiMiGiB; //size in kilobytes
	double Size_MB = Size_kB / kiMiGiB; //size in Megabytes
	double Size_GB = Size_MB / kiMiGiB; //size in Gigabytes

	//check which prefix must be used and copy value to helper1 and 2
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

	//to get the size with always 4 digits and a dot, check which case must be used and compute each digit
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

//function to create the file length char array
//Parameters: len_char = length of the file as char array, len = length in seconds
void FileBrowser::createLenChar(char* len_Char, unsigned long len)
{
	//compute each digit of the time to get structure hh:mm:ss
	len_Char[0] = char(int(len / 3600 / 10) + '0');
	len_Char[1] = char(int(len / 3600 % 10) + '0');
	len_Char[3] = char(int(len / 60 / 10 % 6) + '0');
	len_Char[4] = char(int(len / 60 % 10) + '0');
	len_Char[6] = char(int(len / 10 % 6) + '0');
	len_Char[7] = char(int(len % 10) + '0');
}

