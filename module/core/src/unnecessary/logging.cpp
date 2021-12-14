#include <unnecessary/logging.h>

namespace un {
    std::mutex Log::globalMutex;

    void Log::flush() {
        if (flushed) {
            return;
        }
        flushed = true;
        stream() << std::endl;
        globalMutex.unlock();
    }

    std::ostream& Log::stream() {
        return std::cout;
    }

    Log::~Log() {
        flush();
    }

    Log::Log(
        const std::string& header,
        u32 line,
        const std::string& file
    ) : flushed(false) {
        globalMutex.lock();
        stream() <<
                 DEFAULT_COLOR << '[' <<
                 termcolor::green << header <<
                 DEFAULT_COLOR << "] " <<
                 termcolor::magenta << '<' << file << "#L" << line << '>' <<
                 DEFAULT_COLOR << ": ";
    }
}