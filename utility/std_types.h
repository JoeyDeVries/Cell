#ifndef UTILITY_STD_TYPES_H
#define UTILITY_STD_TYPES_H

#include <cstdint>


/* NOTE(Joey):

  Define a set of types with fixed bit sizes, regardless of
  platform in use. The typedefs specify the fixed types from
  <cstdint> in a more convenient and intuitive manner.

  It's advised to use if necessary, but sparringly. Using
  the 'natural word size' of the machine is and will always
  be faster for the future to come. So only use them when
  doing fixed-size bit operations or having strict type
  size requirements.

  For instance, a loop variable doesn't need a fixed 32 bit
  integer as the minimum integer requirement of 16 bits by
  the C++ specification will suffice.

*/

typedef std::int8_t   i8;
typedef std::int16_t  i16;
typedef std::int32_t  i32;
typedef std::int64_t  i64;

typedef std::uint8_t  u8;
typedef std::uint16_t u16;
typedef std::uint32_t u32;
typedef std::uint64_t u64;

typedef std::uint32_t b32;

typedef float  r32;
typedef double r64;

#endif