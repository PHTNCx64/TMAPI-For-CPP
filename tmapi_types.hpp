/* C++ Wrapper around the Target Manager API. Written by Starman aka PHTNC. */
/* This project is still in development. Modification and redistribution is allowed. */

#ifndef _TMAPI_TYPES_HPP_
#define _TMAPI_TYPES_HPP_

// The size of an int depends on the compiler and target architecture, in order to prevent confusion and possible malfunction of the library,
// I have defined u32 and s32 as a long, as long values are always 32 bits.

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef unsigned long long u64;
typedef signed char s8;
typedef signed short s16;
typedef signed long s32;
typedef signed long long s64;

typedef float f32;
typedef double f64;


#endif // _TMAPI_TYPES_HPP_