//
// Created by brunorbsf on 25/09/22.
//

#ifndef UNNECESSARYENGINE_ENUM_SERIALIZATION_LOGIC_H
#define UNNECESSARYENGINE_ENUM_SERIALIZATION_LOGIC_H

#include <sstream>
#include <unnecessary/serializer/generation.h>
#include <unnecessary/source_analysis/structures.h>

namespace un::serialization {
    void write_enum_serialization_logic(
        std::stringstream& ss,
        const generation_info& info,
        const std::shared_ptr<cxx_enum>& anEnum
    );

    void write_enum_deserialization_logic(
        std::stringstream& ss,
        const generation_info& info,
        const std::shared_ptr<cxx_enum>& anEnum
    );

    void write_enum_serialization_logic_pair(
        std::stringstream& ss,
        const generation_info& info,
        const std::shared_ptr<cxx_enum>& anEnum
    );
}

#endif
