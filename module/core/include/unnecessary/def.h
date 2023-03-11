#ifndef UNNECESSARYENGINE_DEF_H
#define UNNECESSARYENGINE_DEF_H

#include <cstddef>
#include <unnecessary/strings.h>

#ifdef WIN32
using u8 = unsigned __int8;
using u16 = unsigned __int16;
using u32 = unsigned __int32;
using u64 = unsigned __int64;
#else
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long u64;
#endif

using i8 = char;
using i16 = short;
using i32 = int;
using i64 = long;

using f32 = float;
using f64 = double;
#ifdef UN_PLATFORM_UNIX
#define UN_AGGRESSIVE_INLINE __attribute__((always_inline)) inline
#elif UN_PLATFORM_WINDOWS
#define UN_AGGRESSIVE_INLINE __forceinline
#else
#define UN_AGGRESSIVE_INLINE inline
#endif

namespace un {
    template<typename T>
    using ref = T&;

    template<typename T>
    using const_ref = const T&;

    template<typename T>
    using ptr = T*;

    using void_ptr = ptr<void>;

    template<typename T>
    using const_ptr = un::ptr<const T>;
#ifdef UN_PLATFORM_WINDOWS

    UN_AGGRESSIVE_INLINE void debug_break()
    {
        __debugbreak();
    }

#elif UN_PLATFORM_UNIX

    UN_AGGRESSIVE_INLINE void debug_break() {
        __builtin_trap();
    }

#endif
}
#endif
