#include <unnecessary/logging.h>

namespace un {

    void Log::flush() {
        if (flushed) {
            return;
        }
        flushed = true;
        stream() << std::endl;
    }

    std::ostream &Log::stream() {
        return std::cout;
    }

    Log::~Log() {
        flush();
    }

    Log::Log(const std::string &header, u32 line, const std::string &file) : flushed(false) {
        stream() <<
                 DEFAULT_COLOR << '[' <<
                 termcolor::green << header <<
                 DEFAULT_COLOR << "] " <<
                 termcolor::magenta << '<' << file << "#L" << line << '>' <<
                 DEFAULT_COLOR << ": ";
    }
}