//
// Created by bruno on 18/05/2022.
//

#include "enum_writer.h"

namespace un {

    bool EnumWriter::accepts(const CXXField& field, const CXXTranslationUnit& unit, float& outPriority) {
        outPriority = 1.5F;
        return field.getType().getKind() == un::CXXTypeKind::eEnum;
    }

    void EnumWriter::write_serializer(
        std::stringstream& ss,
        const CXXField& field,
        const CXXTranslationUnit& unit,
        const WriterRegistry& registry
    ) {

    }

    void EnumWriter::write_deserializer(
        std::stringstream& ss,
        const CXXField& field,
        const CXXTranslationUnit& unit,
        const WriterRegistry& registry
    ) {

    }

    std::string EnumWriter::name() {
        return "EnumWriter";
    }
}