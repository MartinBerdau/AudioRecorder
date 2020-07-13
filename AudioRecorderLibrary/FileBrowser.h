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

#pragma once


class FileBrowser
{
public:
	FileBrowser();
	~FileBrowser();

	//function to compute and generate the file name
	void computeCurName(char* lastFilename, unsigned int flagUpDown);
	//function to compute and generate the displayable file size char array 
	void computeFileSizeChar(char* Size, unsigned long len_byte);
	//function to compute and generate the displayable file length char array
	void computeFileLenChar(char* Len, unsigned long len_byte);
	//function to compute and generate the displayable available Memory (bytes and time) char array
	void computeAvailableMemory(char* Memory, unsigned long long SDsize_bytes, unsigned long long MemoryUsed_bytes, unsigned long long availableMemory, unsigned long availableTime);
	//function to create the available Memory char array
	void createMemoryChar(char* Memory, unsigned long long availableMemory, unsigned long availableTime);
	//function to create the file size char array
	void createSizeChar(char* size_Char, unsigned long long size);
	//function to create the file length char array
	void createLenChar(char* len_Char, unsigned long len);

private:
	unsigned int m_fs;				//samplingrate
	unsigned int m_bitDepth;		//bit depth
	unsigned int m_nrOfChannels;	//number of channels
	unsigned int m_bytesPerSecond;	//bytes per second
};

