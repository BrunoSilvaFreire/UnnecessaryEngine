//
// Created by bruno on 09/05/2022.
//

#ifndef UNNECESSARYENGINE_DELEGATE_WRITER_H
#define UNNECESSARYENGINE_DELEGATE_WRITER_H

#include "field_writer.h"

namespace un {
    class delegate_writer : public field_writer {
    public:
        bool
        accepts(
            const cxx_field& field,
            const cxx_translation_unit& unit,
            float& outPriority
        ) override;

        void
        write_serializer(
            std::stringstream& ss, const cxx_field& field, const cxx_translation_unit& unit,
            const writer_registry& registry
        ) override;

        void
        write_deserializer(
            std::stringstream& ss, const cxx_field& field, const cxx_translation_unit& unit,
            const writer_registry& registry
        ) override;

        std::string name() override;
    };
}

#endif //UNNECESSARYENGINE_DELEGATE_WRITER_H
