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
}