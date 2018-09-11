#pragma once

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
  * Bits is a small header-only bit-manipulation library.
  *
  * Bit positions are specified using little-endian order; pos == 0 is the least
  * significant bit.
  */

#include <type_traits>

namespace bits {

constexpr unsigned BITS_IN_BYTE = 8;

/**
 * Set a bit in dest at pos.
 */
template <typename DestType>
void setBit(DestType& dest, unsigned pos, bool value) {
  static_assert(std::is_integral<DestType>::value, 
    "DestType must be an unsigned integer type");
  static_assert(std::is_unsigned<DestType>::value, 
    "DestType must be an unsigned integer type");
  dest = (value << pos) | (dest & ~(1 << pos));
}

/**
 * Get a bit from src at pos.
 */
template <typename SrcType>
bool getBit(const SrcType& src, unsigned pos) {
  static_assert(std::is_integral<SrcType>::value,
    "SrcType must be an unsigned integer type");
  static_assert(std::is_unsigned<SrcType>::value,
    "SrcType must be an unsigned integer type");
  return (static_cast<bool>(src & (1 << pos)));
}


template <typename T, bool = std::is_enum<T>::value >
struct resolve_type {
    using type = T;
};

template <typename T>
struct resolve_type<T, true> {
    using type = typename std::underlying_type<T>::type;
};

// Bits computes the static constexpr parameters needed by set and get bits functions.
// It's meant to be used from setBits, getSbits, getUbits, and getBits, but can also be
// used directly if you don't mind specifying all your template parameters.
// It has two implementations, selected as partial template specializations. One is for
// signed types and the other for unsigned.
template <unsigned width, unsigned lsb, typename ValueType, typename StorageType,
            typename ResolvedValueType = typename resolve_type< ValueType >::type,
            bool = std::is_signed< ResolvedValueType >::value>
class Bits {
    // This definition is not used. It will always resolve to either the signed or unsigned implementation.
};


// If ResolvedValueType is signed, use this implementation.
template <unsigned width, unsigned lsb, typename ValueType, typename StorageType, typename ResolvedValueType>
class Bits<width, lsb, ValueType, StorageType, ResolvedValueType, true> {
    using ValueIntType = typename std::make_signed< StorageType >::type;

    static_assert(std::is_integral<StorageType>::value,
        "StorageType must be an unsigned integer type");

    static_assert(std::is_unsigned<StorageType>::value,
        "StorageType must be an unsigned integer type");

    static_assert(std::is_integral<ValueType>::value || std::is_enum<ValueType>::value,
        "ValueType must be an integer or enum type");

    static_assert(width < sizeof(StorageType) * BITS_IN_BYTE,
        "width must be < than sizeof StorageType * BITS_IN_BYTE");

    static_assert(sizeof(StorageType) * BITS_IN_BYTE >= width + lsb,
        "sizeof StorageType * BITS_IN_BYTE must be >= width + lsb");

    static_assert((static_cast<ValueIntType>(-1) >> ((sizeof(StorageType) * BITS_IN_BYTE) - 1)) == -1,
        "This architecture doesn't support sign-extending a negative signed integer with a right shift");

private:
    static constexpr ValueIntType MASK = ((static_cast<ValueIntType>(1) << width) - 1) << lsb;
    static constexpr ValueIntType INVERSE_MASK = ~MASK;
    static constexpr unsigned LEFT_SHIFT = sizeof(StorageType) * BITS_IN_BYTE - lsb - width;
    static constexpr unsigned RIGHT_SHIFT = LEFT_SHIFT + lsb;

public:
    static void Set(StorageType& dest, const ValueType value) {
        dest = ((static_cast<StorageType>(value) << lsb) & MASK) | (dest & INVERSE_MASK);
    }

    static ValueType Get(const StorageType& src) {
        // sign extend, unless you have an evil arch
        return static_cast<ValueType>(static_cast<ValueIntType>((src & MASK) << LEFT_SHIFT) >> RIGHT_SHIFT);
    }
};

// If ResolvedValueType is unsigned, use this implementation.
template <unsigned width, unsigned lsb, typename ValueType, typename StorageType, typename ResolvedValueType>
class Bits<width, lsb, ValueType, StorageType, ResolvedValueType, false> {

    static_assert(std::is_integral<StorageType>::value,
        "StorageType must be an unsigned integer type");

    static_assert(std::is_unsigned<StorageType>::value,
        "StorageType must be an unsigned integer type");

    static_assert(std::is_integral<ValueType>::value || std::is_enum<ValueType>::value,
        "ValueType must be an integer or enum type");

    static_assert(width < sizeof(StorageType) * BITS_IN_BYTE,
        "width must be < than sizeof StorageType * BITS_IN_BYTE");

    static_assert(sizeof(StorageType) * BITS_IN_BYTE >= width + lsb,
        "sizeof StorageType * BITS_IN_BYTE must be >= width + lsb");

private:
    static constexpr StorageType MASK = ((static_cast<StorageType>(1) << width) - 1) << lsb;
    static constexpr StorageType INVERSE_MASK = ~MASK;

public:
    static void Set(StorageType& dest, const ValueType value) {
        dest = ((static_cast<StorageType>(value) << lsb) & MASK) | (dest & INVERSE_MASK);
    }

    static ValueType Get(const StorageType& src) {
        return static_cast<ValueType>(( src & MASK) >> lsb);
    }
};

/**
 * Sets a bit field.
 * Works for enums, unsigned or signed fields.
 * All template parameters except width and lsb can be inferred.
 */
template <unsigned width, unsigned lsb, typename ValueType, typename StorageType>
void setBits(StorageType& dest, const ValueType value){
    Bits<width, lsb, ValueType, StorageType>::Set(dest, value);
}

/**
 * Gets an unsigned bit field.
 * All template parameters except width and lsb can be inferred.
 */
template <unsigned width, unsigned lsb, typename StorageType>
StorageType getUbits(const StorageType& src) {
    return Bits<width, lsb, StorageType, StorageType>::Get(src);
}


/**
 * Gets a signed bit field.
 * All template parameters except width and lsb can be inferred.
 */
template <unsigned width, unsigned lsb, typename StorageType, typename ValueType = typename std::make_signed<StorageType>::type>
ValueType getSbits(const StorageType& src) {
    return Bits<width, lsb, ValueType, StorageType>::Get(src);
}

/**
 * Gets a specified type of bit field. Can be used for an enum, signed, or unsigned field.
 * Must specify the return type as a template parameter, since it can't be inferred, but StorageType can be inferred.
 */
template <unsigned width, unsigned lsb, typename ValueType, typename StorageType>
ValueType getBits(const StorageType& src){
    return Bits<width, lsb, ValueType, StorageType>::Get(src);
}

} // namespace
