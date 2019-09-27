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
// mirroreddelayline-example.cpp
//
//------------------------------------------------------------------------------
//
// This has been tested using cygwin and x86_64-w64-mingw32-g++.
//
// Compile: g++ mirroreddelayline-example.cpp -I ../include -o mirroreddelayline-example.exe -lm -static
// Run: ./mirroreddelayline-example.exe
//
//------------------------------------------------------------------------------

// Includes
#include <ctime>
#include <iostream>
#include "MirroredDelayLine.h"

void doTimingComparisons(size_t delayLineLength, int repetitions);

//! Main Function
int main(int argc, char** argv)
{
	std::cout << "MirroredDelayLine example usage" << std::endl << std::endl;

	size_t delayLineLength = 5;

	std::vector<int> values(delayLineLength*2);
	for( size_t i = 0; i < values.size(); i++ )
	{
		values[i] = i;
	}

	// Create a MirroredDelayLine
	MirroredDelayLine<int> intDelayLine(delayLineLength, 0);

	std::cout << "Initialised delay line: " << std::endl;
	std::cout << "Contents=";
	intDelayLine.debug_printContents();

	std::cout << "Appending." << std::endl;
	for( size_t i = 0; i < values.size(); i++ )
	{
		intDelayLine.append( values[i] );
		std::cout << i << " data()-Contents=[";
		for(size_t index = 0; index < intDelayLine.length(); index++ )
		{
			std::cout << intDelayLine.data()[index];
			if( index < (intDelayLine.length()-1) )
			{
				std::cout << ", ";
			}
		}
		std::cout << "]";

		std::cout << " [i]-Contents=[";
		for(size_t index = 0; index < intDelayLine.length(); index++ )
		{
			std::cout << intDelayLine[index];
			if( index < (intDelayLine.length()-1) )
			{
				std::cout << ", ";
			}
		}
		std::cout << "]" << std::endl;

	}

	std::cout << "Clearing." << std::endl;
	intDelayLine.clear(0);
	std::cout << "Contents=";
	intDelayLine.debug_printContents();

	// Timing Comparisons with naive for(i=0 to length) move, memmove and mirrored delay line.
	//size_t delayLineLength;
	int repetitions;

	delayLineLength = 100;
	repetitions = 100000000;
	doTimingComparisons(delayLineLength, repetitions);

	delayLineLength = 2000;
	repetitions = 10000000;
	doTimingComparisons(delayLineLength, repetitions);

	delayLineLength = 8000;
	repetitions = 10000000;
	doTimingComparisons(delayLineLength, repetitions);

	return 0;
}

void doTimingComparisons(size_t delayLineLength, int repetitions)
{
	//size_t delayLineLength = 2000;
	//int repetitions = 10000000;
	double repetitionsDouble = (double)repetitions;

	// Create the structures
	std::vector<int> intVector(delayLineLength, 0);
	MirroredDelayLine<int> intDelayLine(delayLineLength, 0);

	// The times
	std::clock_t startTime;
	std::clock_t endTime;

	double naiveTimeNs;
	double memmoveTimeNs;
	double mirrorTimeNs;

	std::cout << "Timing comparisons." << std::endl;
	std::cout << "delayLineLength=" << delayLineLength << std::endl;
	std::cout << "repetitions=" << repetitions << std::endl;

	std::cout << "Starting timing comparison: Naive" << std::endl;

	//
	// Naive
	//
	// Start the time
	startTime = std::clock();

	for( int r = 0; r < repetitions; r++ )
	{
		// Append r
		for(size_t i = 0; i < delayLineLength-1; i++)
		{
			intVector[i] = intVector[i+1];
		}
		intVector[delayLineLength-1] = r;
	}

	// Stop the time
	endTime = std::clock();
	// Duration
	naiveTimeNs = (1000000000.0 * (double)(endTime - startTime) / (double)CLOCKS_PER_SEC) / repetitionsDouble;

	std::cout << "Starting timing comparison: Memmove" << std::endl;

	//
	// Memmove
	//
	// Start the time
	startTime = std::clock();

	for( int r = 0; r < repetitions; r++ )
	{
		// Append r
		memmove(&intVector[0], &intVector[1], delayLineLength-1);
		intVector[delayLineLength-1] = r;
	}

	// Stop the time
	endTime = std::clock();
	// Duration
	memmoveTimeNs = (1000000000.0 * (double)(endTime - startTime) / (double)CLOCKS_PER_SEC) / repetitionsDouble;

	std::cout << "Starting timing comparison: Mirrored Delay Line" << std::endl;

	//
	// Mirrored Delay Line
	//
	// Start the time
	startTime = std::clock();

	for( int r = 0; r < repetitions; r++ )
	{
		// Append r
		intDelayLine.append(r);
	}

	// Stop the time
	endTime = std::clock();
	// Duration
	mirrorTimeNs = (1000000000.0 * (double)(endTime - startTime) / (double)CLOCKS_PER_SEC) / repetitionsDouble;

	std::cout << "Time per delay line append operation. " << std::endl;
	std::cout << "Naive = " << naiveTimeNs << " ns" << std::endl;
	std::cout << "Memmove = " << memmoveTimeNs << " ns" << std::endl;
	std::cout << "Mirrored Delay Line = " << mirrorTimeNs << " ns" << std::endl;

	return;
}
