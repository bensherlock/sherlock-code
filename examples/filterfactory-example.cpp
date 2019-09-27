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
// main.cpp
//
//------------------------------------------------------------------------------
//
// This has been tested using cygwin and x86_64-w64-mingw32-g++.
//
// Compile: g++ filterfactory-example.cpp -I ../include -o filterfactory-example.exe -lm -static
// Run: ./filterfactory-example.exe
//
//------------------------------------------------------------------------------

// Includes
#include <iostream>

#include "FilterFactory.h"


//! Main Function
int main(int argc, char** argv)
{
	std::cout << "FilterFactory example usage" << std::endl << std::endl;

	// Create a FilterFactory
	FilterFactory filterFactory;

	// Filter order – must be even number.
	int order = 4;

	// Sample Frequency
	Float fSampling = 48000.0;

	// Cut frequency
	Float fCut = 4000.0;

	// Create Filter
	FilterHolderType lowpassFilter = filterFactory.createFilter(
						order, FilterWindowType::HAMMING, FilterType::LOWPASS,
						fSampling, fCut);

	// Output details
	std::cout << "Lowpass Filter: order=" << order
			  << " fSampling=" << fSampling
			  << "Hz fCut=" << fCut << "Hz" << std::endl;

	std::cout << "coefficients=[";
	for(int n = 0; n < lowpassFilter.coefficients.size(); n++)
	{
		std::cout << lowpassFilter.coefficients[n];

		if( n < (lowpassFilter.coefficients.size()-1) )
		{
			std::cout << ", ";
		}
	}
	std::cout << "]" << std::endl << std::endl;

	// Filter order – must be even number.
	order = 8;

	// Sample Frequency
	fSampling = 48000.0;

	// Cut frequencies
	Float fCutLow = 8000.0;
	Float fCutHigh = 16000.0;

	// Create Filter
	FilterHolderType bandpassFilter = filterFactory.createFilter(
	order, FilterWindowType::HAMMING, FilterType::BANDPASS,
	fSampling, fCutLow, fCutHigh);

	// Output details
	std::cout << "Bandpass Filter: order=" << order
			  << " fSampling=" << fSampling
			  << "Hz fCutLow=" << fCutLow
			  << "Hz fCutHigh=" << fCutHigh << "Hz" << std::endl;

	std::cout << "coefficients=[";
	for(int n = 0; n < bandpassFilter.coefficients.size(); n++)
	{
		std::cout << bandpassFilter.coefficients[n];

		if( n < (bandpassFilter.coefficients.size()-1) )
		{
			std::cout << ", ";
		}
	}
	std::cout << "]" << std::endl << std::endl;

	return 0;
}
