#include <unnecessary/misc/types.h>

namespace un {

    std::string demangle(const char* value) {
        //https://stackoverflow.com/questions/281818/unmangling-the-result-of-stdtype-infoname
        int status = -4;
        std::unique_ptr<char, void (*)(void*)> res{
            abi::__cxa_demangle(value, NULL, NULL, &status),
            std::free
        };

        return (status == 0) ? res.get() : value;
    }
}