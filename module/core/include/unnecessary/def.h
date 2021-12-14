#ifndef UNNECESSARYENGINE_DEF_H
#define UNNECESSARYENGINE_DEF_H

#include <cstddef>
#include <unnecessary/strings.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long u64;

typedef float f32;
typedef double f64;

#ifdef GNU
#defisne UN_AGGRESIVE_INLINE __attribute__((always_inline)) inline
#elif _MSC_VER
#define UN_AGGRESIVE_INLINE inline
#else
#define UN_AGGRESIVE_INLINE inline
#endif
#endif
