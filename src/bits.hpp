#ifndef BITS_HPP
#define BITS_HPP

/* Copyright (C) 2018 Alan Grover
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

 /**
  * Bits is a small header-only bit-manipulation library that works
  * with c++98 but can take advantage of compile-time safety checks
  * on c++11 or better.
  *
  * Bit positions are specified using little-endian order; pos == 0 is the least
  * significant bit.
  */

// Allow it to compile on pre-C++11 compilers
// MSVC reports __cplusplus as being C98 since it doesn't conform to all features of the newer standard(s)
// The MSVC_LANG approximates the compilers capabilities
#ifndef _MSVC_LANG
    #define _MSCV_LANG __cplusplus
#endif

#if __cplusplus < 201103L && _MSVC_LANG < 201103L
    #define constexpr const
    #define static_assert(a,b)
    #include <limits.h>
#else
    #include <type_traits>
    #include <climits>
#endif

namespace bits {

constexpr unsigned BITS_IN_BYTE = CHAR_BIT;

/**
 * Set a bit in dest at pos.
 */
template <unsigned pos, typename DestType>
void setBit(DestType& dest, bool value) {

  static_assert(std::is_integral<DestType>::value,
    "DestType must be an unsigned integer type");

  static_assert(std::is_unsigned<DestType>::value,
    "DestType must be an unsigned integer type");

  static_assert(pos < sizeof(DestType) * BITS_IN_BYTE,
    "pos must be < sizeof(DestType) * BITS_IN_BYTE");

  dest = (static_cast<DestType>(value) << pos) | (dest & ~(static_cast<DestType>(1) << pos));
}

/**
 * Get a bit from src at pos.
 */
template <unsigned pos, typename SrcType>
bool getBit(const SrcType& src) {

  static_assert(std::is_integral<SrcType>::value,
    "SrcType must be an unsigned integer type");

  static_assert(std::is_unsigned<SrcType>::value,
    "SrcType must be an unsigned integer type");

  static_assert(pos < sizeof(SrcType) * BITS_IN_BYTE,
    "pos must be < sizeof(SrcType) * BITS_IN_BYTE");

  return (static_cast<bool>(src & (static_cast<SrcType>(1) << pos)));
}

/**
 * Set a field in dest to value. The size of the field in bits is width and the
 * field's least significant bit is at lsb.
 */
template<unsigned width, unsigned lsb, typename DestType, typename ValueType>
void setBits(DestType& dest, const ValueType value) {
    static_assert(std::is_integral<DestType>::value,
        "DestType must be an unsigned integer type");

    static_assert(std::is_unsigned<DestType>::value,
        "DestType must be an unsigned integer type");

    static_assert(std::is_integral<ValueType>::value
        || std::is_enum<ValueType>::value,
        "ValueType must be an integer or enum type");

    static_assert(width > 0,
        "width must be > 0");

    static_assert(width < sizeof(DestType) * BITS_IN_BYTE,
        "width must be < than sizeof DestType * BITS_IN_BYTE");

    static_assert(sizeof(DestType) * BITS_IN_BYTE >= width + lsb,
        "sizeof DestType * BITS_IN_BYTE must be >= width + lsb");

    static constexpr DestType MASK = ((static_cast<DestType>(1) << width) - 1) << lsb;

    static constexpr DestType INV_MASK = ~MASK;

    dest = (MASK & (static_cast<DestType>(value) << lsb)) | (dest & INV_MASK);
}

/**
 * Get an unsigned field from src. The size of the field in bits is width and the
 * field's least significant bit at lsb.
 */
template<unsigned width, unsigned lsb, typename T>
T getUbits(const T& src){
    static_assert(std::is_integral<T>::value,
        "T must be an unsigned integer type");

    static_assert(std::is_unsigned<T>::value,
        "T must be an unsigned integer type");

    static_assert(width > 0,
        "width must be > 0");

    static_assert(width < sizeof(T) * BITS_IN_BYTE,
        "width must be < than sizeof T * BITS_IN_BYTE");

    static_assert(sizeof(T) * BITS_IN_BYTE >= width + lsb,
        "sizeof T * BITS_IN_BYTE must be >= width + lsb");

    static constexpr T MASK = ((static_cast<T>(1) << width) - 1) << lsb;

    return (MASK & src) >> lsb;
}

/**
 * Get a signed field from src. The size of the field in bits is width and the
 * field's least significant bit at lsb.
 */
template<unsigned width, unsigned lsb, typename ValueType, typename SrcType>
ValueType getSbits(const SrcType& src) {
    static_assert(std::is_integral<SrcType>::value,
        "SrcType must be an unsigned integer type");

    static_assert(std::is_unsigned<SrcType>::value,
        "SrcType must be an unsigned integer type");

    static_assert(std::is_integral<ValueType>::value,
        "ValueType must be a signed integer type");

    static_assert(std::is_signed<ValueType>::value,
        "ValueType must be a signed integer type");

    static_assert(width > 0,
        "width must be > 0");

    static_assert(width < sizeof(SrcType) * BITS_IN_BYTE,
        "width must be < than sizeof SrcType * BITS_IN_BYTE");

    static_assert(sizeof(SrcType) * BITS_IN_BYTE >= width + lsb,
        "sizeof SrcType * BITS_IN_BYTE must be >= width + lsb");

    static_assert(sizeof(ValueType) * BITS_IN_BYTE >= width,
        "sizeof ValueType * BITS_IN_BYTE must be >= width");

    static constexpr SrcType MASK = ((static_cast<SrcType>(1) << width) - 1) << lsb;

    // minimum value in a signed field of width bits
    static constexpr SrcType MIN_VALUE = static_cast<SrcType>(1) << (width - 1);

    SrcType retVal = (MASK & src) >> lsb;
    // sign extend without implementation defined behavior
    // XORing clears the bit at MIN_VALUE;
    // then it uses unsigned integer underflow behavior guaranteed by the
    // standard to fill MIN_VALUE and the bits to the left of it
    return static_cast<ValueType>((retVal ^ MIN_VALUE) - MIN_VALUE);
}

}


#endif
