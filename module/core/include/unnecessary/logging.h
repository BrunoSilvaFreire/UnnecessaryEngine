#ifndef UNNECESSARYENGINE_LOGGING_H
#define UNNECESSARYENGINE_LOGGING_H

#include <iostream>
#include <mutex>
#include <unnecessary/def.h>
#include <sstream>
#include <termcolor/termcolor.hpp>

namespace un {
    class log {
    private:
        static std::mutex globalMutex;
        bool _flushed;
        std::stringstream _sstream;

    public:
        void flush();

        log(const std::string& header, const std::string& label);

        log(const std::string& header, u32 line, const std::string& file);

        ~log();

        std::ostream& stream();
    };

    enum class severity {
        debug,
        info,
        warn,
        error,
        fatal
    };
}

#define VERBOSE "VERBOSE"
#define INFO "INFO"
#define WARN "WARN"
#define FUCK "OH SHIT OH FUCK"

#define DEFAULT_COLOR termcolor::reset
#define COLORED(MSG, COLOR) COLOR << MSG << DEFAULT_COLOR

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wstring-plus-int"

#endif
#ifdef DEBUG
//#define UN_USE_ABS_PATH_FOR_LOGGING
#endif

#ifdef UN_USE_ABS_PATH_FOR_LOGGING
#ifndef UN_CURRENT_FILE
#define UN_CURRENT_FILE __FILE__
#endif
#else
#ifdef __clang__

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wstring-plus-int"
#endif

#ifdef UN_CMAKE_FILE_NAME
#define UN_CURRENT_FILE UN_CMAKE_FILE_NAME
#else
#define UN_CURRENT_FILE __FILE__
#endif
#endif

#ifdef __clang__

#pragma clang diagnostic pop
#endif
#define GREEN(MSG) COLORED(MSG, termcolor::green)
#define RED(MSG) COLORED(MSG, termcolor::red)
#define PURPLE(MSG) COLORED(MSG, termcolor::magenta)
#define YELLOW(MSG) COLORED(MSG, termcolor::yellow)
#define LOG(LEVEL) un::log(LEVEL, __LINE__, UN_CURRENT_FILE).stream()

#endif
