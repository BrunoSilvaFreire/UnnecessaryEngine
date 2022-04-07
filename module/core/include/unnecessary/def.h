#ifndef UNNECESSARYENGINE_DEF_H
#define UNNECESSARYENGINE_DEF_H

#include <cstddef>
#include <unnecessary/strings.h>

#ifdef WIN32
typedef unsigned __int8 u8;
typedef unsigned __int16 u16;
typedef unsigned __int32 u32;
typedef unsigned __int64 u64;
#else
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long u64;
#endif

typedef char i8;
typedef short i16;
typedef int i32;
typedef long i64;

typedef float f32;
typedef double f64;

#ifdef GNU
#define UN_AGGRESSIVE_INLINE __attribute__((always_inline)) inline
#elif _MSC_VER
#define UN_AGGRESSIVE_INLINE __forceinline
#else
#define UN_AGGRESSIVE_INLINE inline
#endif
#endif
