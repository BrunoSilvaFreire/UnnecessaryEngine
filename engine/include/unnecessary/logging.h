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
#define SHIT "SHIT"

#define COLORED(MSG, COLOR) COLOR << MSG << termcolor::grey

#define GREEN(MSG) COLORED(MSG, termcolor::green)
#define RED(MSG) COLORED(MSG, termcolor::red)
#define PURPLE(MSG) COLORED(MSG, termcolor::magenta)
#define LOG(LEVEL) Log(LEVEL, __LINE__, __FILE__).stream()
#endif
