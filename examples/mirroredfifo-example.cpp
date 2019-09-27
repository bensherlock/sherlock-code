//------------------------------------------------------------------------------
// The MIT License (MIT)
// 
// Copyright (c) 2015 Benjamin Sherlock
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//------------------------------------------------------------------------------
//
// mirroredfifo-example.cpp
//
//------------------------------------------------------------------------------
//
// This has been tested using cygwin and x86_64-w64-mingw32-g++.
//
// Compile: g++ mirroredfifo-example.cpp -I ../include -o mirroredfifo-example.exe -lm -static
// Run: ./mirroredfifo-example.exe
//
//------------------------------------------------------------------------------

// Includes
#include <iostream>
#include "MirroredFifo.h"

//! Main Function
int main(int argc, char** argv)
{
	std::cout << "MirroredFifo example usage" << std::endl << std::endl;

	size_t fifoLength = 10;
	size_t canReadCount = 0;
	size_t canWriteCount = 0;
	size_t readCount = 0;
	size_t writeCount = 0;

	std::vector<int> values(fifoLength);
	std::vector<int> readValues(fifoLength, 0);
	for( size_t i = 0; i < fifoLength; i++ )
	{
		values[i] = i;
	}

	// Create a MirroredFifo
	MirroredFifo<int> intFifo(fifoLength);

	// Fill and Read
	canReadCount = intFifo.canRead();
	canWriteCount = intFifo.canWrite();

	std::cout << "Empty fifo: canReadCount=" << canReadCount
			  << " canWriteCount=" << canWriteCount << std::endl;

	writeCount = intFifo.write(1, &values[0]);

	canReadCount = intFifo.canRead();
	canWriteCount = intFifo.canWrite();

	std::cout << "One entry in fifo: writeCount=" << writeCount
			  << " canReadCount=" << canReadCount
			  << " canWriteCount=" << canWriteCount << std::endl;
	std::cout << "Contents: ";
	intFifo.debug_printContents();

	writeCount = intFifo.write(fifoLength, &values[0]);

	canReadCount = intFifo.canRead();
	canWriteCount = intFifo.canWrite();

	std::cout << "Full fifo: writeCount=" << writeCount
			  << " canReadCount=" << canReadCount
			  << " canWriteCount=" << canWriteCount << std::endl;
	std::cout << "Contents: ";
	intFifo.debug_printContents();

	readCount = intFifo.read(1, &readValues[0]);

	canReadCount = intFifo.canRead();
	canWriteCount = intFifo.canWrite();

	std::cout << "One below full fifo: readCount=" << readCount
			  << " canReadCount=" << canReadCount
			  << " canWriteCount=" << canWriteCount << std::endl;
	std::cout << "Contents: ";
	intFifo.debug_printContents();

	readCount = intFifo.read(fifoLength, &readValues[0]);

	canReadCount = intFifo.canRead();
	canWriteCount = intFifo.canWrite();

	std::cout << "Empty fifo: readCount=" << readCount
			  << " canReadCount=" << canReadCount
			  << " canWriteCount=" << canWriteCount << std::endl;
	std::cout << "Contents: ";
	intFifo.debug_printContents();

	writeCount = intFifo.write(fifoLength/2, &values[0]);

	canReadCount = intFifo.canRead();
	canWriteCount = intFifo.canWrite();

	std::cout << "Half Full fifo: writeCount=" << writeCount
			  << " canReadCount=" << canReadCount
			  << " canWriteCount=" << canWriteCount << std::endl;
	std::cout << "Contents: ";
	intFifo.debug_printContents();

	readCount = intFifo.read(fifoLength, &readValues[0]);

	canReadCount = intFifo.canRead();
	canWriteCount = intFifo.canWrite();

	std::cout << "Empty fifo: readCount=" << readCount
			  << " canReadCount=" << canReadCount
			  << " canWriteCount=" << canWriteCount << std::endl;
	std::cout << "Contents: ";
	intFifo.debug_printContents();

	writeCount = intFifo.write(fifoLength, &values[0]);

	canReadCount = intFifo.canRead();
	canWriteCount = intFifo.canWrite();

	std::cout << "Looped Full fifo: writeCount=" << writeCount
			  << " canReadCount=" << canReadCount
			  << " canWriteCount=" << canWriteCount << std::endl;
	std::cout << "Contents: ";
	intFifo.debug_printContents();

	readCount = intFifo.read(fifoLength, &readValues[0]);

	canReadCount = intFifo.canRead();
	canWriteCount = intFifo.canWrite();

	std::cout << "Empty fifo: readCount=" << readCount
			  << " canReadCount=" << canReadCount
			  << " canWriteCount=" << canWriteCount << std::endl;
	std::cout << "Contents: ";
	intFifo.debug_printContents();

	writeCount = intFifo.write(fifoLength, &values[0]);

	canReadCount = intFifo.canRead();
	canWriteCount = intFifo.canWrite();

	std::cout << "Full fifo: writeCount=" << writeCount
			  << " canReadCount=" << canReadCount
			  << " canWriteCount=" << canWriteCount << std::endl;
	std::cout << "Contents: ";
	intFifo.debug_printContents();

	intFifo.clear();

	canReadCount = intFifo.canRead();
	canWriteCount = intFifo.canWrite();

	std::cout << "Cleared Empty fifo: "
			  << " canReadCount=" << canReadCount
			  << " canWriteCount=" << canWriteCount << std::endl;
	std::cout << "Contents: ";
	intFifo.debug_printContents();

	// Now try the overwriting version

	writeCount = intFifo.write(fifoLength/2, &values[0]);

	canReadCount = intFifo.canRead();
	canWriteCount = intFifo.canWrite();

	std::cout << "Half Full fifo: writeCount=" << writeCount
			  << " canReadCount=" << canReadCount
			  << " canWriteCount=" << canWriteCount << std::endl;
	std::cout << "Contents: ";
	intFifo.debug_printContents();

	// Now write over again
	writeCount = intFifo.write(fifoLength, &values[0], true);

	canReadCount = intFifo.canRead();
	canWriteCount = intFifo.canWrite();

	std::cout << "Full Overwritten fifo: writeCount=" << writeCount
			  << " canReadCount=" << canReadCount
			  << " canWriteCount=" << canWriteCount << std::endl;
	std::cout << "Contents: ";
	intFifo.debug_printContents();

	return 0;
}