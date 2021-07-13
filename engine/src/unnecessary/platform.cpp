#include <unnecessary/platform.h>

#ifdef WIN32

void* un::loadDynamicModule(const std::string& moduleName) {
    return GetModuleHandle(static_cast<LPCSTR>(moduleName.c_str()));
}

#endif