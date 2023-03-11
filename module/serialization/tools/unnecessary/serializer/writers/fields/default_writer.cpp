#include "default_writer.h"

namespace un {
    bool
    primitive_writer::accepts(
        const cxx_field& field,
        const cxx_translation_unit& unit,
        float& outPriority
    ) {
        outPriority = 0;
        return true;
    }

    void primitive_writer::write_deserializer(
        std::stringstream& ss, const cxx_field& field,
        const cxx_translation_unit& unit,
        const writer_registry& registry
    ) {
        std::string fName = field.get_name();
        std::string typeName = field.get_type().get_name();
        bool optional = is_optional(field);
        if (optional) {
            ss << "from.try_get<" << typeName << ">" << "(\"" << fName << "\", value." << fName
               << "); // Optional"
               << std::endl;
        }
        else {
            ss << "if (!from.try_get<" << typeName << ">"
               << "(\"" << fName << "\", value." << fName << ")) {" << std::endl;
            add_missing_field_exception(ss, field);
            ss << "}" << std::endl;
        }
    }

    void primitive_writer::write_serializer(
        std::stringstream& ss,
        const cxx_field& field,
        const cxx_translation_unit& unit,
        const writer_registry& registry
    ) {
        try_serialize_primitive(ss, field.get_name(), field.get_type());
    }

    std::string primitive_writer::name() {
        return "PrimitiveWriter";
    }
}
