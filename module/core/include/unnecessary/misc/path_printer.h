//
// Created by bruno on 06/08/2022.
//

#ifndef UNNECESSARYENGINE_PATH_PRINTER_H
#define UNNECESSARYENGINE_PATH_PRINTER_H

#include <string>
#include <filesystem>

namespace un {
    std::string uri(const std::filesystem::path& path);
    std::string prettify(const std::filesystem::path& path);
}
#endif
