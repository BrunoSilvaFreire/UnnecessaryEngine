#include <unnecessary/strings.h>

namespace un {
    std::string capitalize(std::string value) {
        value[0] = toupper(value[0]);
        return value;
    }

    std::string upper(std::string value) {
        std::string uppercased;
        uppercased.resize(value.size());
        for (std::size_t i = 0; i < value.size(); ++i) {
            uppercased[i] = toupper(value[i]);
        }
        return uppercased;
    }

    std::string lower(std::string value) {
        std::string uppercased;
        uppercased.resize(value.size());
        for (std::size_t i = 0; i < value.size(); ++i) {
            uppercased[i] = tolower(value[i]);
        }
        return uppercased;
    }

    std::vector<std::string> split_string(const std::string& str, const std::string& delimiter) {
        size_t pos = 0;
        std::string s = str;
        std::string token;
        std::vector<std::string> vec;
        while ((pos = s.find(delimiter)) != std::string::npos) {
            token = s.substr(0, pos);
            s.erase(0, pos + delimiter.length());
            vec.push_back(token);
        }
        if (!s.empty()) {
            vec.push_back(s);
        }
        return vec;
    }

    std::string trim_whitespace_prefix(const std::string& value) {
        std::string result = value;
        if (auto res = result.find_last_of(' '); res != std::string::npos) {
            result.erase(0, res);
        }
        return result;
    }

#ifndef UN_PLATFORM_WINDOWS
    template<>
    std::string to_string(const std::filesystem::path& value) {
        return value.string();
    }
#endif
}
