#ifndef UNNECESSARYENGINE_STRINGS_H
#define UNNECESSARYENGINE_STRINGS_H

#include <string>
#include <sstream>

namespace un {
    template<typename T>
    std::string to_string(const T& value) {
        return std::to_string(value);
    }

    std::string capitalize(std::string value);

    std::string upper(std::string value);

    std::string lower(std::string value);

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
