//
// Created by brunorbsf on 08/05/22.
//

#ifndef UNNECESSARYENGINE_GENERATION_H
#define UNNECESSARYENGINE_GENERATION_H

#include <string>
#include <filesystem>
#include <unnecessary/strings.h>
#include <unnecessary/logging.h>
#include <unnecessary/misc/files.h>
#include <unnecessary/source_analysis/cppast/transpilation.h>

namespace un {
    struct generation_info {
        std::string name;
        std::string fullName;
        std::string upper;
        std::string lower;

        explicit generation_info(
            const cxx_declaration& toGenerate
        );
    };
}
#endif //UNNECESSARYENGINE_GENERATION_H
