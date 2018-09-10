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
  *
  * WARNING: Behavior is undefined for bit positions and field widths that are
  *          not in the source or destination type. No runtime checks are
  *          performed to validate bit positions and field widths.
  */


#include <type_traits>
#include <tuple>
#include <utility>
#include <stdexcept>

#include <typeinfo>
#include <iostream>

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

// Names of the elements of a maskShifts tuple.
constexpr size_t MASK = 0;
constexpr size_t LEFT_SHIFT = 1;
constexpr size_t RIGHT_SHIFT = 2;

template <typename T>
using maskShift = std::pair<T, unsigned>;

/**
 * Make the mask for a bit field, which can be done at compile time. The name,
 * which was going to be awkward no matter what, is meant to help document
 * the parameters. Also passes the lsb along so setBits and getUbits don't need
 * lsb as an additional parameter, with the possibility of parameter mismatch. 
 * Intended to be used with setBits and getUbits.
 */
template <typename T>
constexpr maskShift<T> width_lsb(unsigned width, unsigned lsb) {
  return {
    ((static_cast<T>(~0) >> (sizeof(T) * BITS_IN_BYTE - width))) << lsb, // MASK
    lsb
  };
}


template <typename T>
using maskShifts = std::tuple<T, unsigned, unsigned>;

/**
 * Make the mask and shift constants, which can be done at compile time.
 * The name, which was going to be awkward no matter what, is meant to help 
 * document the parameters but be shorter since the rest of the getSbits call
 * is longer.
 * Intended to be used with getSbits.
 */
template <typename T>
constexpr maskShifts<T> w_lsb(
    unsigned width,
    unsigned lsb) {
  return maskShifts<T> {
    width_lsb<T>(width, lsb).first,         // MASK
    sizeof(T) * BITS_IN_BYTE - lsb - width, // LEFT_SHIFT
    sizeof(T) * BITS_IN_BYTE - width, // RIGHT_SHIFT = LEFT_SHIFT + lsb
  };
}

/**
 * Set a bit field in dest. The bit-field is defined by the lsb (least-
 * significant bit position) and width. Both signed and unsigned fields
 * can be set with this function.
 * Because of type inference, only the type parameter for width_lsb must be
 * specified.
 * E.g.:
 * int value = -1;
 * uint32_t dest = 0;
 * bits::setBits(dest, bits::width_lsb<uint32_t>(3, 0), value);
 */
template <typename DestType, typename ValueType=DestType>
void setBits(DestType& dest, const maskShift<DestType> m_s, const ValueType value) {
  static_assert(std::is_integral<DestType>::value,
    "DestType must be an unsigned integer type");
  static_assert(std::is_unsigned<DestType>::value,
    "DestType must be an unsigned integer type");
  static_assert(std::is_integral<ValueType>::value | std::is_enum<ValueType>::value,
    "ValueType must be an integer type or enum");
  static_assert(sizeof(ValueType) <= sizeof(DestType),
    "ValueType must be <= DestType");
  // m_s.first is the mask
  // m_s.second is the lsb
  dest = ((static_cast<DestType>(value) << m_s.second) & m_s.first) 
	  | (dest & ~m_s.first);
}

/**
 * Get an unsigned bit-field from src. The bit-field is defined by the lsb
 * (least-significant bit position) and width.
 * Because of type inference, only the type parameter for width_lsb must be
 * specified.
 * E.g.:
 * uint32_t src = 0xA0000F45;
 * assert(bits::getUbits(src, bits::width_lsb<uint32_t>(9, 0)) == 0x145);
 */
template <typename SrcType>
SrcType getUbits(const SrcType& src, const maskShift<SrcType> m_s) {
  static_assert(std::is_integral<SrcType>::value,
    "SrcType must be an unsigned integer type");
  static_assert(std::is_unsigned<SrcType>::value,
    "SrcType must be an unsigned integer type");
  // m_s.first is the mask
  // m_s.second is the lsb
  return ( (src & m_s.first) >> m_s.second );
}

/**
 * Get a signed bit-field from src. The bit-field is defined by the lsb
 * (least-significant bit position) and width.
 * Because of type inference, only the type parameter for the return value
 * and w_lsb must be specified.
 * E.g.:
 * uint32_t src = 0xA0000F45;
 * assert(bits::getSbits<int32_t>(src, bits::w_lsb<uint32_t>(9, 0)) == -187);
 */
template <typename ValueType, typename SrcType>
ValueType getSbits(const SrcType& src, const maskShifts<SrcType> m_s) {
  static_assert(std::is_integral<SrcType>::value,
    "SrcType must be an unsigned integer type");
  static_assert(std::is_unsigned<SrcType>::value,
    "SrcType must be an unsigned integer type");
  static_assert(std::is_integral<ValueType>::value,
    "ValueType must be a signed integer type");
  static_assert(std::is_signed<ValueType>::value,
    "ValueType must be a signed integer type");
  static_assert(sizeof(ValueType) == sizeof(SrcType),
    "ValueType must be the same size as SrcType");
  return (
    static_cast<ValueType>(
      (src & std::get<MASK>(m_s)) << std::get<LEFT_SHIFT>(m_s)
    ) >> std::get<RIGHT_SHIFT>(m_s) // sign extend, unless you have an evil arch
  );
}

template <typename T, bool = std::is_enum<T>::value >
struct resolve_type {
    using type = T;
};

template <typename T>
struct resolve_type<T, true> {
    using type = typename std::underlying_type<T>::type;
};

// Get a signed (or unsigned) bit field. Meant to be used from getSbits so that
// template parameters can be inferred, but can also be used directly.
template <unsigned width, unsigned lsb, typename ValueType, typename StorageType>
class SBits {
    using ResolvedValueType = typename resolve_type< ValueType >::type;
    using ValueIntType = typename std::conditional< std::is_signed< ResolvedValueType >::value,
                                    typename std::make_signed< StorageType >::type,
                                    StorageType >::type;

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

    static_assert((static_cast<typename std::make_signed<StorageType>::type>(-1) >> ((sizeof(StorageType) * BITS_IN_BYTE) - 1)) == -1,
        "This architecture doesn't support sign-extending a negative signed integer with a right shift");

private:
	static constexpr ValueIntType MASK = ((1 << width) - 1) << lsb;
	static constexpr ValueIntType INVERSE_MASK = ~MASK;
	static constexpr unsigned LEFT_SHIFT = sizeof(StorageType) * BITS_IN_BYTE - lsb - width;
	static constexpr unsigned RIGHT_SHIFT = LEFT_SHIFT + lsb;

public:
	static ValueType Get(const StorageType& src) {
	    // sign extend, unless you have an evil arch
	    return static_cast<ValueType>( ((static_cast<ValueIntType>(src & MASK)) << LEFT_SHIFT) >> RIGHT_SHIFT );
	}
};


// Get an unsigned bit field. Meant to be used from getUbits and setBits so that
// template parameters can be inferred, but can also be used directly.
template <unsigned width, unsigned lsb, typename ValueType, typename StorageType>
class UBits {

    static_assert(std::is_integral<StorageType>::value,
        "StorageType must be an unsigned integer type");

    static_assert(std::is_unsigned<StorageType>::value,
        "StorageType must be an unsigned integer type");

    static_assert(std::is_integral<ValueType>::value || std::is_enum<ValueType>::value,
        "ValueType must be an integer enum type");

    static_assert(width < sizeof(StorageType) * BITS_IN_BYTE,
        "width must be < than sizeof StorageType * BITS_IN_BYTE");

    static_assert(sizeof(StorageType) * BITS_IN_BYTE >= width + lsb,
        "sizeof StorageType * BITS_IN_BYTE must be >= width + lsb");

private:
    static constexpr StorageType MASK = ((1 << width) - 1) << lsb;
    static constexpr StorageType INVERSE_MASK = ~MASK;

public:
    static void Set(StorageType& dest, const ValueType value) {
        dest = ((static_cast<StorageType>(value) << lsb) & MASK) | (dest & INVERSE_MASK);
    }

    static ValueType Get(const StorageType& src) {
        return ( src & MASK) >> lsb;
    }
};


template <unsigned width, unsigned lsb, typename ValueType, typename StorageType>
void setBits(StorageType& dest, const ValueType value){
    UBits<width, lsb, ValueType, StorageType>::Set(dest, value);
}

/**
 * Gets an unsigned bit field.
 * All template parameters except width and lsb can be inferred.
 */
template <unsigned width, unsigned lsb, typename StorageType>
StorageType getUbits(const StorageType& src) {
    return UBits<width, lsb, StorageType, StorageType>::Get(src);
}


/**
 * Gets a signed bit field.
 * All template parameters except width and lsb can be inferred.
 */
template <unsigned width, unsigned lsb, typename StorageType, typename ValueType = typename std::make_signed<StorageType>>
ValueType getSbits(const StorageType& src) {
    return SBits<width, lsb, ValueType, StorageType>::Get(src);
}

/**
 * Gets an enum bit field.
 * Can specify the return type of the Enum as a template parameter, since it can't be inferred.
 */
template <unsigned width, unsigned lsb, typename EnumType, typename StorageType>
EnumType getEbits(const StorageType& src){
    return SBits<width, lsb, EnumType, StorageType>::Get(src);
}



} // namespace
