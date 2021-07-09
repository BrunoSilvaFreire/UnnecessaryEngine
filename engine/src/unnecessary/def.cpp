#include <unnecessary/def.h>

void assertVkCall(vk::Result result, std::string method) {
    if (result != vk::Result::eSuccess) {
        std::string str = "Error while calling ";
        str += method;
        str += " (";
        str += vk::to_string(result);
        str += ")";
        throw std::runtime_error(str);
    }
}