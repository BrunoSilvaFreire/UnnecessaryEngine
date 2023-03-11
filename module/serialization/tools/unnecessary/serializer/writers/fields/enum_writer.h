//
// Created by brunorbsf on 25/09/22.
//

#ifndef UNNECESSARYENGINE_ENUM_WRITER_H
#define UNNECESSARYENGINE_ENUM_WRITER_H

#include "field_writer.h"

namespace un {
    class enum_by_name_writer : public field_writer {
    public:
        bool
        accepts(
            const cxx_field& field,
            const cxx_translation_unit& unit,
            float& outPriority
        ) override;

        void write_serializer(
            std::stringstream& ss,
            const cxx_field& field,
            const cxx_translation_unit& unit,
            const writer_registry& registry
        ) override;

        void write_deserializer(
            std::stringstream& ss,
            const cxx_field& field,
            const cxx_translation_unit& unit,
            const writer_registry& registry
        ) override;

        std::string name() override;
    };
}

#endif //UNNECESSARYENGINE_ENUM_WRITER_H
