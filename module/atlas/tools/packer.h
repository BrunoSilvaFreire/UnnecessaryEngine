#ifndef UNNECESSARYENGINE_PACKER_H
#define UNNECESSARYENGINE_PACKER_H

#include <cxxopts.hpp>
#include <png++/png.hpp>
#include <unnecessary/def.h>

namespace un::packer {
    void pack(const std::vector<std::string>& files);
}
#endif
