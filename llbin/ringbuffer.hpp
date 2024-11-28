
//-------------------------------------------------------------------------------------------------
//
//  ringbuffer.hpp
//
//  Simple fixed size ring buffer.
//  Manage objects by value.
//  Thread safe for single Producer and single Consumer.
//
//-------------------------------------------------------------------------------------------------
//
// Author: Dennis Lang - 2021
// https://landenlabs.com
//
// This file is part of llbin project.
//
// ----- License ----
//
// Copyright (c) 2021  Dennis Lang
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is furnished to do
// so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


#pragma once
typedef unsigned int sizeT;
#include <assert.h>

template <class T>
class RingBuffer {
public:
    RingBuffer(sizeT size = 100)
        : m_size(size), m_buffer(new T[size]), m_rIndex(0), m_wIndex(0)
    { assert(size > 1 && m_buffer != nullptr); }

    ~RingBuffer()
    { delete [] m_buffer; };

    sizeT Next(sizeT n) const
    { return (n + 1) % m_size; }
    bool Empty() const
    { return (m_rIndex == m_wIndex); }
    bool Full() const
    { return (Next(m_wIndex) == m_rIndex); }

    bool Put( T& value) {
        if (Full())
            return false;
        m_buffer[m_wIndex] = value;
        m_wIndex = Next(m_wIndex);
        return true;
    }

    bool Get(T& value) {
        if (Empty())
            return false;
        value = m_buffer[m_rIndex];
        m_rIndex = Next(m_rIndex);
        return true;
    }

private:
    T*             m_buffer;
    sizeT          m_size;

    // volatile is only used to keep compiler from placing values in registers.
    // volatile does NOT make the index thread safe.
    volatile sizeT m_rIndex;
    volatile sizeT m_wIndex;
};
