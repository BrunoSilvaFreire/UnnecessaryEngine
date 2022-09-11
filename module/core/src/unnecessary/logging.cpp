#include <unnecessary/logging.h>
#include <filesystem>
#include <vector>

namespace un {
    std::mutex Log::globalMutex;

    void Log::flush() {
        if (flushed) {
            return;
        }
        flushed = true;
        {
            std::lock_guard<std::mutex> lock(globalMutex);
            std::cout << sstream.str() << std::endl;
        }
    }

    std::ostream& Log::stream() {
        return sstream;
    }

    Log::~Log() {
        flush();
    }

/*    std::string select_filename(const std::string& file) {
        std::filesystem::path path(file);
        std::vector<std::string> parts;
        for (auto part: path) {
            parts.emplace_back(part.string());
        }
        std::string str;
        std::size_t first = parts.size() - 1;
        for (int i = first; i >= 0; --i) {
            auto part = parts[i];
            if (part == "src" || part == "include") {
                break;
            }
            if (i != first) {
                str = std::filesystem::path::preferred_separator + str;
            }
            str = part + str;
        }
        return str;
    }*/

    Log::Log(
        const std::string& header,
        u32 line,
        const std::string& file
    ) : flushed(false) {
        sstream <<
                DEFAULT_COLOR << '[' <<
                termcolor::green << header <<
                DEFAULT_COLOR << "] " <<
                termcolor::magenta << '<' << file << ":" << line
                << '>' <<
                DEFAULT_COLOR << ": ";
    }

    Log::Log(const std::string& header, const std::string& label) : flushed(false) {
        sstream <<
                DEFAULT_COLOR << '[' <<
                termcolor::green << header <<
                DEFAULT_COLOR << "] " <<
                termcolor::magenta << '<' << label
                << '>' <<
                DEFAULT_COLOR << ": ";
    }
}