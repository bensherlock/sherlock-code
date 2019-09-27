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
// MirroredDelayLine.h
//
//------------------------------------------------------------------------------
//
// https://bensherlock.co.uk/2015/09/15/mirrored-delay-line/
//
//------------------------------------------------------------------------------
//
// It uses a variation on the "virtual ring buffer" technique, though instead of 
// mapped memory it is a direct copy in contiguous memory. This allows block reads 
// from contiguous memory.
// http://atastypixel.com/blog/circular-ring-buffer-plus-neat-virtual-memory-mapping-trick/
// https://fgiesen.wordpress.com/2012/07/21/the-magic-ring-buffer/
// 
//------------------------------------------------------------------------------

#ifndef MIRROREDDELAYLINE_H
#define MIRROREDDELAYLINE_H

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>

template <typename DataType>
class MirroredDelayLine
{
public:

    //! Constructor 
    //! delayLineLength = the length of the delay line
    MirroredDelayLine(size_t delayLineLength);
    
    //! Constructor 
    //! delayLineLength = the length of the delay line.
    //! clearValue = the value to initialise the contents to.
    MirroredDelayLine(size_t delayLineLength, const DataType& clearValue);
    
    //! Destructor
    virtual ~MirroredDelayLine();
    
    //! Clear the delay line
    //! clearValue = the value to initialise the contents to.
    void clear(const DataType& clearValue);
    
    //! Get the length of the delay line
    size_t length();
    
    //! Get a pointer to the current head of the  delay line
    const DataType * const data();
    
    //! Append data to end of delay line (and lose the first item)
    void append(const DataType &data);
    
    //! Array Subscript Operator Overload
    const DataType& operator[](size_t index);
    

    //! Debug: Print the contents to std::cout 
    void debug_printContents();
    
protected:

private:	
    //! Usable Delay Line Length
    size_t m_delayLineLength;
    
    //! Total Storage Length
    size_t m_totalStorageLength;
	
    //! Storage Vector
    std::vector<DataType> m_storage;
    
    //! Index 
    size_t m_index;

}; // class MirroredDelayLine


//! Constructor
template <typename DataType>
MirroredDelayLine<DataType>::MirroredDelayLine(size_t delayLineLength)
    : m_delayLineLength(delayLineLength), m_totalStorageLength(2*m_delayLineLength), 
      m_storage(m_totalStorageLength), m_index(0)
{
}


//! Constructor 
//! delayLineLength = the length of the delay line.
//! clearValue = the value to initialise the contents to.
template <typename DataType>
MirroredDelayLine<DataType>::MirroredDelayLine(size_t delayLineLength, const DataType& clearValue)
    : m_delayLineLength(delayLineLength), m_totalStorageLength(2*m_delayLineLength), 
      m_storage(m_totalStorageLength, clearValue), m_index(0)
{
}


//! Destructor
template <typename DataType>
MirroredDelayLine<DataType>::~MirroredDelayLine() 
{
}


//! Clear the delay line
//! clearValue = the value to initialise the contents to.
template <typename DataType>
void MirroredDelayLine<DataType>::clear(const DataType& clearValue) 
{
    m_index = 0;
    memset(&m_storage[0], clearValue, m_totalStorageLength*sizeof(DataType));
}


//! Get the length of the delay line
template <typename DataType>
size_t MirroredDelayLine<DataType>::length()
{
    return m_delayLineLength;
}


//! Get a pointer to the current head of the  delay line
template <typename DataType>
const DataType * const MirroredDelayLine<DataType>::data()
{
    return &m_storage[m_index];
}


//! Append data to end of delay line (and lose the first item)
template <typename DataType>
void MirroredDelayLine<DataType>::append(const DataType &data)
{
    m_storage[m_index] = data;
    m_storage[m_index+m_delayLineLength] = data;
    
    m_index++;
    if( m_index == m_delayLineLength )
    {
        // Wrap around
        m_index = 0;
    }
}


//! Array Subscript Operator Overload - read only
template <typename DataType>
const DataType& MirroredDelayLine<DataType>::operator[](size_t index)
{
    return m_storage[index + m_index];
}


//! Debug: Print the contents to std::cout 
template <typename DataType>
void MirroredDelayLine<DataType>::debug_printContents()
{
    std::cout << "[";
    
    for( size_t i = 0; i < m_delayLineLength; i++ )
    {
        std::cout << m_storage[m_index+i];
        
        if( i < (m_delayLineLength-1) )
        {
            std::cout << ", ";
        }
    }
    
    std::cout << "]" << std::endl;
}


#endif // MIRROREDDELAYLINE_H


