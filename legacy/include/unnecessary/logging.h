#ifndef UNNECESSARYENGINE_LOGGING_H
#define UNNECESSARYENGINE_LOGGING_H

#include <iostream>
#include <termcolor/termcolor.hpp>
#include <mutex>
#include "def.h"

namespace un {
    class Log {
    private:
        static std::mutex globalMutex;

        bool flushed;
    public:
        void flush();

        Log(const std::string& header, u32 line, const std::string& file);

        ~Log();

        std::ostream& stream();
    };
}
#define INFO "INFO"
#define WARN "WARN"
#define FUCK "OH SHIT OH FUCK"

#if defined(_WIN32)
#define DEFAULT_COLOR termcolor::white
#else
#define DEFAULT_COLOR termcolor::grey
#endif
#define COLORED(MSG, COLOR) COLOR << MSG << DEFAULT_COLOR

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wstring-plus-int"

#endif

#define UN_USE_ABS_PATH_FOR_LOGGING 1

#ifdef UN_USE_ABS_PATH_FOR_LOGGING
#define __FILENAME__ __FILE__
#else

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wstring-plus-int"
#endif

//#define __FILENAME__ (__FILE__ + SOURCE_PATH_SIZE)

#ifdef __clang__
#pragma clang diagnostic pop
#endif
#endif

#define GREEN(MSG) COLORED(MSG, termcolor::green)
#define RED(MSG) COLORED(MSG, termcolor::red)
#define PURPLE(MSG) COLORED(MSG, termcolor::magenta)
#define YELLOW(MSG) COLORED(MSG, termcolor::yellow)
#define LOG(LEVEL) un::Log(LEVEL, __LINE__, __FILE__).stream()


#endif