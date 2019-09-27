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
// MirroredFifo.h
//
//------------------------------------------------------------------------------
//
// https://bensherlock.co.uk/2015/09/14/mirrored-fifo/
//
//------------------------------------------------------------------------------
//
// https://en.wikipedia.org/wiki/Circular_buffer
// This FIFO uses the "Always keep one slot open" strategy for managing the 
// full/empty buffer distinction.
//
// It uses a variation on the "virtual ring buffer" technique, though instead of 
// mapped memory it is a direct copy in contiguous memory. This allows block reads 
// from contiguous memory.
// http://atastypixel.com/blog/circular-ring-buffer-plus-neat-virtual-memory-mapping-trick/
// https://fgiesen.wordpress.com/2012/07/21/the-magic-ring-buffer/
// 
//------------------------------------------------------------------------------

#ifndef MIRROREDFIFO_H
#define MIRROREDFIFO_H

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>

template <typename DataType>
class MirroredFifo
{
public:

    //! Constructor 
    //! fifoLength = the maximum number of items to be stored
    MirroredFifo(size_t fifoLength);
    
    //! Destructor
    virtual ~MirroredFifo();
    
    
    //! Clear the fifo
    void clear();
    
    //! How many values we can read from the fifo.
    size_t canRead();
    
    //! How many values we can write to the fifo.
    size_t canWrite();
    
    //! Write data to fifo - array - returns number of items written
    size_t write( size_t length, const DataType * const data, bool overwrite = false );
    
    //! Write data to fifo - single item - returns number of items written
    size_t writeOne( const DataType &data, bool overwrite = false );
    
    //! Read data from the fifo - array - returns number of items read
    size_t read( size_t length, DataType * data );

    //! Read data from the fifo - single item - returns item read
    DataType readOne( );
    
    

    //! Debug: Print the contents to std::cout 
    void debug_printContents();
    
protected:

private:	

    //! Usable Fifo Length
    size_t m_fifoLength;
    
    //! Total Storage Length
    size_t m_totalStorageLength;
	
    //! Storage Vector
    std::vector<DataType> m_storage;
    
    //! Head Index (Read from the head)
    volatile size_t m_headIndex;
    
    //! Tail Index (Write to the tail)
    volatile size_t m_tailIndex;
	
}; // class MirroredFifo


//! Constructor
template <typename DataType>
MirroredFifo<DataType>::MirroredFifo(size_t fifoLength)
    : m_fifoLength(fifoLength+1), m_totalStorageLength(2*m_fifoLength), 
      m_storage(m_totalStorageLength), m_headIndex(0), m_tailIndex(0)
{
}

//! Destructor
template <typename DataType>
MirroredFifo<DataType>::~MirroredFifo() 
{
}

//! Clear the fifo
template <typename DataType>
void MirroredFifo<DataType>::clear() 
{
    m_headIndex = 0;
    m_tailIndex = 0;
}

//! How many values we can read from the fifo.
template <typename DataType>
size_t MirroredFifo<DataType>::canRead() 
{
    size_t canRead = 0;
    if( m_headIndex == m_tailIndex ) {
        canRead = 0;
    } else if( m_tailIndex > m_headIndex ) {
        canRead = m_tailIndex - m_headIndex;
    } else {
        canRead = (m_tailIndex + m_fifoLength - m_headIndex);
    }
    return canRead;
}

//! How many values we can write to the fifo.
template <typename DataType>
size_t MirroredFifo<DataType>::canWrite() 
{
    //size_t canRead = 0;
    size_t canWrite = 0;
    if( m_headIndex == m_tailIndex ) {
        //canRead = 0;
        canWrite = m_fifoLength - 1;
    } else if( m_tailIndex > m_headIndex ) {
        //canRead = m_tailIndex - m_headIndex;
        //canWrite = m_fifoLength - canRead - 1;
        canWrite = m_fifoLength - ( m_tailIndex - m_headIndex ) - 1;
    } else {
        //canRead = (m_tailIndex + m_fifoLength - m_headIndex);
        //canWrite = m_fifoLength - canRead - 1;
        canWrite = m_fifoLength - (m_tailIndex + m_fifoLength - m_headIndex) - 1;
    }
    return canWrite;
}


//! Write data to fifo - array
template <typename DataType>
size_t MirroredFifo<DataType>::write( size_t length, const DataType * const data, bool overwrite )
{
    size_t canWriteCount = canWrite();
    
    if( !overwrite && ( length > canWriteCount ) )
    {
        length = canWriteCount;
    }
    
    size_t i = 0;
    // First part in the empty fifo slots
    for( i = 0; i < length && i < canWriteCount; i++ )
    {
        // Write to the tail - and mirror
        m_storage[m_tailIndex] = data[i];
        m_storage[m_tailIndex+m_fifoLength] = data[i];
        
        // Increment
        m_tailIndex++;
        if( m_tailIndex == m_fifoLength)
        {
            // Wrap around
            m_tailIndex = 0;
        }
    }

    // Now into the non-empty slots
    for( i; i < length; i++)
    {
        // Write to the tail - and mirror
        m_storage[m_tailIndex] = data[i];
        m_storage[m_tailIndex+m_fifoLength] = data[i];
        
        // Increment
        m_tailIndex++;
        if( m_tailIndex == m_fifoLength)
        {
            // Wrap around
            m_tailIndex = 0;
        }
        
        // Also increment the head counter
        m_headIndex++;
        if( m_headIndex == m_fifoLength)
        {
            // Wrap around
            m_headIndex = 0;
        }
    }

    return length;
}
    
//! Write data to fifo - single item
template <typename DataType>
size_t MirroredFifo<DataType>::writeOne( const DataType &data, bool overwrite )
{
    size_t canWriteCount = canWrite();
    
    if( !overwrite && (canWriteCount == 0) )
    {
        return 0;
    }
    
    // Write to the tail - and mirror
    m_storage[m_tailIndex] = data;
    m_storage[m_tailIndex+m_fifoLength] = data;
    
    // Increment
    m_tailIndex++;
    if( m_tailIndex == m_fifoLength)
    {
        // Wrap around
        m_tailIndex = 0;
    }
    
    if( canWriteCount == 0 )
    {
        // Also increment the head counter
        m_headIndex++;
        if( m_headIndex == m_fifoLength)
        {
            // Wrap around
            m_headIndex = 0;
        }
    }
    
    return 1;
}


//! Read data from the fifo - array - returns number of items read
template <typename DataType>
size_t MirroredFifo<DataType>::read( size_t length, DataType * data )
{
    size_t canReadCount = canRead();
    
    if( canReadCount == 0 ) 
    {
        return 0;
    }
    
    if( length > canReadCount )
    {
        length = canReadCount;
    }

    // Copy from fifo to buffer
    // Read from the head
    memcpy( data, &m_storage[m_headIndex], length*sizeof(DataType) );
    
    // Move the head index
    m_headIndex += length;
    
    if( m_headIndex >= m_fifoLength )
    {
        // Wrap around
        m_headIndex -= m_fifoLength;
    }
    
    return length;
}


//! Read data from the fifo - array - returns number of items read
template <typename DataType>
DataType MirroredFifo<DataType>::readOne()
{
    // Read from the head
    DataType thing = m_storage[m_headIndex];

    // Move the head index
    m_headIndex++;

    if( m_headIndex >= m_fifoLength )
    {
        // Wrap around
        m_headIndex -= m_fifoLength;
    }

    return thing;
}

//! Debug: Print the contents to std::cout 
template <typename DataType>
void MirroredFifo<DataType>::debug_printContents()
{
    size_t canReadCount = canRead();
    
    std::cout << "[";
    
    for( size_t i = 0; i < canReadCount; i++ )
    {
        std::cout << m_storage[m_headIndex+i];
        
        if( i < (canReadCount-1) )
        {
            std::cout << ", ";
        }
    }
    
    std::cout << "]" << std::endl;
}


#endif // MIRROREDFIFO_H


