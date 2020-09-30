#ifndef UNNECESSARYENGINE_LOGGING_H
#define UNNECESSARYENGINE_LOGGING_H

#include <iostream>
#include <termcolor/termcolor.hpp>
#include "def.h"

namespace un {
    class Log {
    private:
        bool flushed;
    public:
        void flush();

        Log(const std::string &header, u32 line, const std::string &file);

        ~Log();

        std::ostream &stream();
    };
}
#define INFO "INFO"
#define WARN "WARN"
#define FUCK "OH SHIT OH FUCK"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wstring-plus-int"
#define COLORED(MSG, COLOR) COLOR << MSG << termcolor::grey
#define __FILENAME__ (__FILE__ + SOURCE_PATH_SIZE)
#define GREEN(MSG) COLORED(MSG, termcolor::green)
#define RED(MSG) COLORED(MSG, termcolor::red)
#define PURPLE(MSG) COLORED(MSG, termcolor::magenta)
#define LOG(LEVEL) Log(LEVEL, __LINE__, __FILENAME__).stream()
#pragma clang diagnostic pop

#endif
