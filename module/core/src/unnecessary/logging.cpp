#include <unnecessary/logging.h>
#include <filesystem>

namespace un {
    std::mutex log::globalMutex;

    void log::flush() {
        if (_flushed) {
            return;
        }
        _flushed = true;
        {
            std::lock_guard<std::mutex> lock(globalMutex);
            std::cout << _sstream.str() << std::endl;
        }
    }

    std::ostream& log::stream() {
        return _sstream;
    }

    log::~log() {
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

    log::log(
        const std::string& header,
        u32 line,
        const std::string& file
    ) : _flushed(false) {
        _sstream << termcolor::colorize <<
                 DEFAULT_COLOR << '[' <<
                 termcolor::green << header <<
                 DEFAULT_COLOR << "] " <<
                 termcolor::magenta << '<' << file << ":" << line
                 << '>' <<
                 DEFAULT_COLOR << ": ";
    }

    log::log(const std::string& header, const std::string& label) : _flushed(false) {
        _sstream << termcolor::colorize <<
                 DEFAULT_COLOR << '[' <<
                 termcolor::green << header <<
                 DEFAULT_COLOR << "] " <<
                 termcolor::magenta << '<' << label
                 << '>' <<
                 DEFAULT_COLOR << ": ";
    }
}
