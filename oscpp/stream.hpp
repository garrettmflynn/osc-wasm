// oscpp library
//
// Copyright (c) 2004-2013 Stefan Kersten <sk@k-hornz.de>
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
//
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#ifndef OSCPP_STREAM_HPP_INCLUDED
#define OSCPP_STREAM_HPP_INCLUDED

#include <oscpp/error.hpp>
#include <oscpp/host.hpp>
#include <oscpp/types.hpp>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>

namespace OSC
{
    class Stream
    {
    public:
        Stream()
        {
            m_begin = m_end = m_pos = 0;
        }

        Stream(void* data, size_t size)
        {
            m_begin = static_cast<byte_t*>(data);
            m_end   = m_begin + size;
            m_pos   = m_begin;
        }

        Stream(const Stream& stream)
        {
            m_begin = m_pos = stream.m_pos;
            m_end = stream.m_end;
        }

        Stream(const Stream& stream, size_t size)
        {
            m_begin = m_pos = stream.m_pos;
            m_end = m_begin + size;
            if (m_end > stream.m_end) throw UnderrunError();
        }

        void reset()
        {
            m_pos = m_begin;
        }

        const byte_t* begin() const
        {
            return m_begin;
        }

        byte_t* begin()
        {
            return m_begin;
        }

        const byte_t* end() const
        {
            return m_end;
        }

        size_t capacity() const
        {
            return end() - begin();
        }
            
        const byte_t* pos() const
        {
            return m_pos;
        }

        byte_t* pos()
        {
            return m_pos;
        }

        template <typename T> inline const T* pointer() const
        {
            return reinterpret_cast<const T*>(pos());
        }

        template <typename T> inline T* pointer()
        {
            return reinterpret_cast<T*>(pos());
        }

        template <typename T> inline const T& ref() const
        {
            return *pointer<T>();
        }

        template <typename T> inline T& ref()
        {
            return *pointer<T>();
        }

        void setPos(void* pos)
        {
            assert((pos >= m_begin) && (pos < m_end));
            m_pos = static_cast<byte_t*>(pos);
        }

        void advance(size_t n)
        {
            m_pos += n;
        }

        bool atEnd() const
        {
            return pos() == end();
        }

        size_t consumed() const
        {
            return pos() - begin();
        }

        size_t consumable() const
        {
            return end() - pos();
        }

        static bool isAligned(size_t n)
        {
            return (n & 3) == 0;
        }

        static size_t align(size_t n)
        {
            return (n + 3) & -4;
        }

        static size_t padding(size_t n)
        {
            return align(n) - n;
        }

    protected:
        byte_t* m_begin;
        byte_t* m_end;
        byte_t* m_pos;
    };

    class WriteStream: public Stream
    {
    public:
        WriteStream()
            : Stream()
        { }

        WriteStream(void* data, size_t size)
            : Stream(data, size)
        { }

        WriteStream(const WriteStream& stream)
            : Stream(stream)
        { }

        WriteStream(const WriteStream& stream, size_t size)
            : Stream(stream, size)
        { }

        // throw (OverflowError)
        void checkWritable(size_t n) const
        {
            if (consumable() < n)
                throw OverflowError(n - consumable());
        }

        void skip(size_t n)
        {
            checkWritable(n);
            advance(n);
        }
        
        void zero(size_t n)
        {
            checkWritable(n);
            memset(m_pos, 0, n);
            advance(n);
        }

        void putChar(char c)
        {
            checkWritable(1);
            ref<char>() = c;
            advance(1);
        }

        void putInt32(int32_t x)
        {
            checkWritable(4);
            ref<int32_t>() = convert32<NetworkByteOrder>(x);
            advance(4);
        }

        void putUInt64(uint64_t x)
        {
            checkWritable(8);
            ref<uint64_t>() = convert64<NetworkByteOrder>(x);
            advance(8);
        }

        void putFloat32(float f)
        {
            checkWritable(4);
            const int32_t x = *reinterpret_cast<int32_t*>(&f);
            ref<float>() = convert32<NetworkByteOrder>(x);
            advance(4);
        }

        void putData(const void* data, size_t size)
        {
            const size_t padding = Stream::padding(size);
            const size_t n = size + padding;
            checkWritable(n);
            memcpy(pos(),      data, size);
            memset(pos()+size, 0,    padding);
            advance(n);
        }

        void putString(const char* s)
        {
            putData(s, strlen(s)+1);
        }
    };


    class ReadStream : public Stream
    {
    public:
        ReadStream()
        { }

        ReadStream(const void* data, size_t size)
            : Stream(const_cast<void*>(data), size)
        { }

        ReadStream(const ReadStream& stream)
            : Stream(stream)
        { }

        ReadStream(const ReadStream& stream, size_t size)
            : Stream(stream, size)
        { }

        // throw (UnderrunError)
        void checkReadable(size_t n) const
        {
            if (consumable() < n) throw UnderrunError();
        }

        // throw (UnderrunError)
        void skip(size_t n)
        {
            checkReadable(n);
            advance(n);
        }

        // throw (UnderrunError)
        inline char peekChar() const
        {
            checkReadable(1);
            return ref<char>();
        }

        // throw (UnderrunError)
        inline char getChar()
        {
            const char x = peekChar();
            advance(1);
            return x;
        }

        // throw (UnderrunError)
        inline int32_t peekInt32() const
        {
            checkReadable(4);
            return convert32<NetworkByteOrder>(ref<int32_t>());
        }

        // throw (UnderrunError)
        inline int32_t getInt32()
        {
            checkReadable(4);
            const int32_t x = convert32<NetworkByteOrder>(ref<int32_t>());
            advance(4);
            return x;
        }

        // throw (UnderrunError)
        inline uint64_t getUInt64()
        {
            checkReadable(8);
            uint64_t x = convert64<NetworkByteOrder>(ref<uint64_t>());
            advance(8);
            return x;
        }

        // throw (UnderrunError)
        inline float getFloat32()
        {
            checkReadable(4);
            uint32_t x = convert32<NetworkByteOrder>(ref<uint32_t>());
            advance(4);
            return *reinterpret_cast<float*>(&x);
        }

        // throw (UnderrunError, ParseError) 
        const char* getString()
        {
            checkReadable(4); // min string length

            const char* ptr = static_cast<const char*>(pos()) + 3;
            const char* end = static_cast<const char*>(this->end());

            while (true) {
                if (ptr >= end) throw UnderrunError();
                if (*ptr == '\0') break;
                ptr += 4;
            }

            const char* x = pos();
            advance(ptr - pos() + 1);

            return x;
        }
    };
}; // namespace OSC

#endif // OSCPP_STREAM_HPP_INCLUDED
