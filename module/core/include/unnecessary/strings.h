#ifndef UNNECESSARYENGINE_STRINGS_H
#define UNNECESSARYENGINE_STRINGS_H

#include <string>
#include <sstream>
#include <vector>
#include <filesystem>

namespace un {
    template<typename T>
    std::string to_string(const T& value) {
        std::stringstream ss;
        ss << value;
        return ss.str();
    }

    template<>
    std::string to_string(const std::filesystem::path& value);

    template<typename T>
    T from_string(const std::string& value);

    std::string capitalize(std::string value);

    std::string upper(std::string value);

    std::string lower(std::string value);

    std::string trim_whitespace_prefix(const std::string& value);

    std::vector<std::string> split_string(const std::string& str, const std::string& delimiter);

    template<typename TIterator, typename TTransform>
    std::string join_strings(TIterator begin, TIterator end, const TTransform& transform) {
        std::stringstream ss;
        TIterator it = begin;
        for (; it != end; it++) {
            ss << transform(*it);
            if (it != end - 1) {
                ss << ", ";
            }
        }
        return ss.str();
    }

    template<typename TIterator>
    std::string join_strings(const std::string& separator, TIterator begin, TIterator end) {
        std::stringstream ss;
        TIterator it = begin;
        for (; it != end; it++) {
            ss << *it;
            if (std::distance(it, end) > 1) {
                ss << separator;
            }
        }
        return ss.str();
    }

    template<typename TIterator>
    std::string join_strings(TIterator begin, TIterator end) {
        return join_strings(", ", begin, end);
    }

}
#endif
