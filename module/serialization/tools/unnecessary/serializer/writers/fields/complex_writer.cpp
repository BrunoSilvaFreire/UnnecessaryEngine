#include "complex_writer.h"

namespace un {
    bool
    complex_writer::accepts(
        const cxx_field& field,
        const cxx_translation_unit& unit,
        float& outPriority
    ) {
        if (field.get_type().get_name() == "std::string") {
            return false;
        }
        outPriority = 1.0F;
        return field.get_type().get_kind() == complex;
    }

    void complex_writer::write_serializer(
        std::stringstream& ss, const cxx_field& field, const cxx_translation_unit& unit,
        const writer_registry& registry
    ) {
        std::string fName = field.get_name();
        std::string typeName = field.get_type().get_name();
        bool optional = is_optional(field);
        ss << "un::Serialized serialized_" << fName << ";" << std::endl;
        ss << "un::serialization::serialize_structure<" << typeName << ">("
           << "value." << field.get_name() << ", " << "serialized_" << fName
           << ");" << std::endl;

        ss << "into.set<un::Serialized>(\"" << fName << "\", serialized_" << fName << ");"
           << std::endl;
    }

    void
    complex_writer::write_deserializer(
        std::stringstream& ss, const cxx_field& field, const cxx_translation_unit& unit,
        const writer_registry& registry
    ) {
        std::string fName = field.get_name();
        std::string typeName = field.get_type().get_name();
        bool optional = is_optional(field);
        ss << "un::Serialized serialized_" << fName << ";" << std::endl;
        if (optional) {
            ss << "if (from.try_get(\"" << fName << "\", serialized_" << fName << ")) {"
               << std::endl;
            ss << "value." << fName << " = un::serialization::deserialize_structure<" << typeName
               << ">(serialized_" <<
               fName
               << ");" << std::endl;
            ss << "}" << std::endl;
        }
        else {
            ss << "if (!from.try_get(\"" << fName << "\", serialized_"
               << fName << ")) {" << std::endl;
            ss << "throw std::runtime_error(\"Unable to read field " << fName << "\");"
               << std::endl;
            ss << "}" << std::endl;
            ss << "value." << fName << " = un::serialization::deserialize_structure<"
               << typeName << ">(serialized_" << fName << ");" << std::endl;
        }
    }

    std::string complex_writer::name() {
        return "ComplexWriter";
    }
}
