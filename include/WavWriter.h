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
// WavWriter.h
//
//------------------------------------------------------------------------------

#ifndef WAVWRITER_H
#define WAVWRITER_H

#include <stdint.h>
#include <stdio.h>
#include <string>
#include <stdint.h>
#include <stdlib.h>

#include <iostream>


// Wave File Header
// http://en.wikipedia.org/wiki/Resource_Interchange_File_Format
// RIFF files consist entirely of "chunks".
// All chunks have the following format:
// * 4 bytes: an ASCII identifier for this chunk (examples are "fmt " and "data"; note the space in "fmt ").
// * 4 bytes: an unsigned, little-endian 32-bit integer with the length of this chunk (except this field itself and the chunk identifier).
//			  variable-sized field: the chunk data itself, of the size given in the previous field.
// * a pad byte, if the chunk's length is not even.
//
// http://en.wikipedia.org/wiki/WAV
// http://www-mmsp.ece.mcgill.ca/documents/audioformats/wave/wave.html
// WAVE file definition (Simplified for use here):
// "RIFF" Chunk
// {
//   "WAVE"
//   {
//     "FMT " Chunk
//     {
//	   }
//     "DATA" Chunk
//     {
//     }
//   }
// }
struct wavutil_header
{
    char riffTag[4]; // FOURCC ("RIFF")
    uint32_t riffChunkLength;

    char waveTag[4]; // FOURCC ("WAVE")
    //uint32_t waveChunkLength; // Doesn't exist?!

    char fmtTag[4]; // FOURCC ("FMT ")
    uint32_t fmtChunkLength; // 16

    // Format Chunk Fields
    uint16_t audioFormat; // PCM = 0x0001 / IEEE Float = 0x0003
    uint16_t numChannels;
    uint32_t sampleRate; // blocks per second
    uint32_t byteRate; // data rate
    uint16_t blockAlign; // data block size (bytes)
    uint16_t bitsPerSample;

    char dataTag[4]; // FOURCC ("DATA")
    uint32_t dataChunkLength;

};

#define WAVE_FORMAT_PCM				0x0001
#define WAVE_FORMAT_IEEE_FLOAT		0x0003



//! Wav Writer Class
class WavWriter {
public:
    //! Constructor
    WavWriter();

    //! Destructor
    virtual ~WavWriter();




    //! Static: Write Header
    static void writeHeader(FILE *file, uint16_t audioFormat, uint16_t numChannels, uint32_t sampleRate, uint16_t bitsPerSample);

    //! Static: Write data bytes
    static void writeData(FILE *file, uint8_t data[], uint32_t length);



    //! Static: Write Mono Wav File with 16-bit Signed Integers
    static void writeWav16(int16_t* data, uint32_t sampleCount, uint32_t sampleRate, std::string filename);
	
	//! Static: Write Mono Wav File with 32-bit Signed Integers
    static void writeWav32(int32_t* data, uint32_t sampleCount, uint32_t sampleRate, std::string filename);
	
	//! Static: Write Mono Wav File with 32-bit Floats
    static void writeWavFloat32(float* data, uint32_t sampleCount, uint32_t sampleRate, std::string filename);
	
	
	//! Static: Read Mono Wav File with 16-bit Signed Integers
	static std::vector<int16_t> readWav16(std::string filename);

protected:

    //! Static: Create Empty Header Struct
    static struct wavutil_header createEmptyHeader();

private:

    void writeLittleEndian(uint32_t word, uint32_t num_bytes, FILE *wav_file);

};



//! Constructor
WavWriter::WavWriter()
{

}

//! Destructor
WavWriter::~WavWriter()
{

}


void WavWriter::writeHeader(FILE *file, uint16_t audioFormat, uint16_t numChannels, uint32_t sampleRate, uint16_t bitsPerSample)
{
    struct wavutil_header header = createEmptyHeader();

    header.audioFormat = audioFormat;//0x0001; // 0x0001=PCM / 0x0003=IEEE float
    header.numChannels = numChannels;
    header.sampleRate = sampleRate;
    header.byteRate = sampleRate * (bitsPerSample/8) * numChannels;
    header.blockAlign = (bitsPerSample/8) * numChannels;
    header.bitsPerSample = bitsPerSample;

    if( !file ) return;

    fwrite(&header, sizeof(header), 1, file);
    fflush(file);
}


// Write data bytes
void WavWriter::writeData(FILE *file, uint8_t data[], uint32_t length)
{
    // Append Data
    if( !file ) return;

    fwrite(data, sizeof(uint8_t), length, file);
    fflush(file);

    // Then seek the header and update the data length field
    size_t fileLength = ftell(file);

    uint32_t dataChunkLength = fileLength - sizeof(struct wavutil_header);
    //uint32_t waveChunkLength = fileLength - 8 - 8;
    uint32_t riffChunkLength = fileLength - 8;

    // Write the dataChunkLength field
    fseek(file, sizeof(struct wavutil_header)-sizeof(uint32_t), SEEK_SET);
    fwrite(&dataChunkLength, sizeof(dataChunkLength), 1, file);

    // Write the riffChunkLength field
    fseek(file, 4, SEEK_SET);
    fwrite(&riffChunkLength, sizeof(riffChunkLength), 1, file);
    //fflush(file);

    // Seek to the end
    fseek(file, fileLength, SEEK_SET);

    fflush(file);
}


struct wavutil_header WavWriter::createEmptyHeader()
{
    struct wavutil_header header;

    // Clear to zeros
    memset(&header, 0, sizeof(struct wavutil_header));

    strncpy(header.riffTag, "RIFF", 4 );
    strncpy(header.waveTag, "WAVE", 4 );
    strncpy(header.fmtTag, "fmt ", 4 );
    strncpy(header.dataTag, "data", 4 );

    header.riffChunkLength = sizeof(struct wavutil_header) - 8;
    //header.waveChunkLength = sizeof(struct wavutil_header) - 8 - 8;
    header.fmtChunkLength = 16; // Fixed for this version

    return header;
}


//! Static: Write Mono Wav File with 16-bit Signed Integers
void WavWriter::writeWav16(int16_t* data, uint32_t sampleCount, uint32_t sampleRate, std::string filename)
{
    FILE* file;

    // Open the file
    file = fopen(filename.c_str(), "wb+");

    if( !file )
    {
        // Error
        return;
    }

    // Write the header
    WavWriter::writeHeader(file, WAVE_FORMAT_PCM, 1, sampleRate, 16);

    // Write the samples in LittleEndian
    WavWriter::writeData(file, (uint8_t*)data, sampleCount * sizeof(int16_t) );

    // Close the file
    fclose(file);
}



//! Static: Write Mono Wav File with 32-bit Signed Integers
void WavWriter::writeWav32(int32_t* data, uint32_t sampleCount, uint32_t sampleRate, std::string filename)
{
    FILE* file;

    // Open the file
    file = fopen(filename.c_str(), "wb+");

    if( !file )
    {
        // Error
        return;
    }

    // Write the header
    WavWriter::writeHeader(file, WAVE_FORMAT_PCM, 1, sampleRate, 32);

    // Write the samples in LittleEndian
    WavWriter::writeData(file, (uint8_t*)data, sampleCount * sizeof(int32_t) );

    // Close the file
    fclose(file);
}



//! Static: Write Mono Wav File with 32-bit Floats
void WavWriter::writeWavFloat32(float* data, uint32_t sampleCount, uint32_t sampleRate, std::string filename)
{
    FILE* file;

    // Open the file
    file = fopen(filename.c_str(), "wb+");

    if( !file )
    {
        // Error
        return;
    }

    // Write the header
    WavWriter::writeHeader(file, WAVE_FORMAT_IEEE_FLOAT, 1, sampleRate, 32);

    // Write the samples in LittleEndian
    WavWriter::writeData(file, (uint8_t*)data, sampleCount * sizeof(float) );

    // Close the file
    fclose(file);
}


void WavWriter::writeLittleEndian(uint32_t word, uint32_t num_bytes, FILE *wav_file)
{
    uint8_t buf;
    while(num_bytes>0)
    {
        buf = word & 0xff;
        fwrite(&buf, 1,1, wav_file);
        num_bytes--;
        word >>= 8;
    }
}

//! Static: Read Mono Wav File with 16-bit Signed Integers
std::vector<int16_t> WavWriter::readWav16(std::string filename)
{
	std::vector<int16_t> samples;
	
	struct wavutil_header header;

    // Clear to zeros
    memset(&header, 0, sizeof(struct wavutil_header));
	
	
	// Open File
	FILE* file;

    // Open the file
    file = fopen(filename.c_str(), "rb");

    if( !file )
    {
        // Error
        return samples;
    }
	
	// Read Header
	fread(&header, sizeof(header), 1, file);
	
	// Check Header details
	if( header.audioFormat != 0x0001 ) // PCM
	{
		std::cout << "Unhandled: audioFormat=" << header.audioFormat << std::endl;
		return samples;
	}
	
    if( header.numChannels != 1 )
	{
		std::cout << "Unhandled: numChannels=" << header.numChannels << std::endl;
		return samples;
	}
	
    //header.sampleRate = sampleRate;
    //header.byteRate = sampleRate * (bitsPerSample/8);
    //header.blockAlign = (bitsPerSample/8);
	if( header.bitsPerSample != 16 )
	{
		std::cout << "Unhandled: bitsPerSample=" << header.bitsPerSample << std::endl;
		return samples;
	}
	
	
	
	// Read data
	// dataChunkLength
	samples.resize( header.dataChunkLength / sizeof(int16_t), 0 );
	fread(&samples[0], sizeof(uint8_t), header.dataChunkLength, file);
	
	fclose(file);
	
	return samples;
}

#endif //WAVWRITER_H
