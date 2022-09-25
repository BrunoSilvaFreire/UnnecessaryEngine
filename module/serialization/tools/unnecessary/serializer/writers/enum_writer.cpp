//
// Created by brunorbsf on 25/09/22.
//

#include "enum_writer.h"

namespace un {

    bool EnumWriter::accepts(const CXXField& field, const CXXTranslationUnit& unit, float& outPriority) {
        outPriority = 10;
        return field.getType().getKind() == un::CXXTypeKind::eEnum;
    }

    void EnumWriter::write_serializer(
        std::stringstream& ss,
        const CXXField& field,
        const CXXTranslationUnit& unit,
        const WriterRegistry& registry
    ) {
        ss << "un::serialization::serialize_inline<" << field.getType().getFullName() << ">(\"" << field.getName()
           << "\", value, into);" << std::endl;
    }

    void EnumWriter::write_deserializer(
        std::stringstream& ss,
        const CXXField& field,
        const CXXTranslationUnit& unit,
        const WriterRegistry& registry
    ) {
        ss << "return un::serialization::deserialize_inline<" << field.getType().getFullName() << ">(\""
           << field.getName()
           << "\", from);" << std::endl;
    }

    std::string EnumWriter::name() {
        return "EnumWriter";
    }
}