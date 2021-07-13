
#ifndef UNNECESSARYENGINE_PLATFORM_H
#define UNNECESSARYENGINE_PLATFORM_H

#include <string>

#ifdef WIN32
#include <windows.h>
#endif
namespace un {
    void* loadDynamicModule(const std::string& moduleName);
}
#endif