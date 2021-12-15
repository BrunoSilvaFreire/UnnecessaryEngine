#include <unnecessary/logging.h>
#include <filesystem>
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
        std::filesystem::path path(file);

        stream() <<
                 DEFAULT_COLOR << '[' <<
                 termcolor::green << header <<
                 DEFAULT_COLOR << "] " <<
                 termcolor::magenta << '<' << path.filename() << ":" << line << '>' <<
                 DEFAULT_COLOR << ": ";
    }
}