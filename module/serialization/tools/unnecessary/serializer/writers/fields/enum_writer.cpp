//
// Created by brunorbsf on 25/09/22.
//

#include "enum_writer.h"

namespace un {
    bool
    enum_by_name_writer::accepts(
        const cxx_field& field,
        const cxx_translation_unit& unit,
        float& outPriority
    ) {
        outPriority = 10;
        return field.get_type().get_kind() == eEnum;
    }

    void enum_by_name_writer::write_serializer(
        std::stringstream& ss,
        const cxx_field& field,
        const cxx_translation_unit& unit,
        const writer_registry& registry
    ) {
        ss << "un::serialization::serialize_inline<" << field.get_type().get_full_name() << ">(\""
           << field
               .get_name()
           << "\", value." << field.get_name() << ", into);" << std::endl;
    }

    void enum_by_name_writer::write_deserializer(
        std::stringstream& ss,
        const cxx_field& field,
        const cxx_translation_unit& unit,
        const writer_registry& registry
    ) {
        ss << "return un::serialization::deserialize_inline<" << field.get_type().get_full_name()
           << ">(\""
           << field.get_name()
           << "\", from);" << std::endl;
    }

    std::string enum_by_name_writer::name() {
        return "EnumByNameWriter";
    }
}
