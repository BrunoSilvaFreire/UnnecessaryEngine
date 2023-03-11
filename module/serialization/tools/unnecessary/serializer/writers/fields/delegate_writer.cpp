//
// Created by bruno on 09/05/2022.
//

#include "delegate_writer.h"

namespace un {
    bool
    delegate_writer::accepts(
        const cxx_field& field,
        const cxx_translation_unit& unit,
        float& outPriority
    ) {
        outPriority = 50;
        const cxx_attribute& att = field.get_attribute("un::serialize");
        return att.has_argument("serializer") && att.has_argument("deserializer");
    }

    void
    delegate_writer::write_serializer(
        std::stringstream& ss, const cxx_field& field, const cxx_translation_unit& unit,
        const writer_registry& registry
    ) {
        const cxx_attribute& att = field.get_attribute("un::serialize");
        std::string serializer = att.get_argument_value("serializer");
        ss << serializer << "(value." << field.get_name() << ", into);" << std::endl;
    }

    void
    delegate_writer::write_deserializer(
        std::stringstream& ss, const cxx_field& field, const cxx_translation_unit& unit,
        const writer_registry& registry
    ) {
        const cxx_attribute& att = field.get_attribute("un::serialize");
        std::string deserializer = att.get_argument_value("deserializer");
        ss << "un::Serialized serialized_" << field.get_name() << ";" << std::endl;
        ss << "if (from.try_get(\"" << field.get_name() << "\", serialized_" << field.get_name()
           << ")) {" << std::endl;
        ss << "value." << field.get_name() << " = " << deserializer << "(serialized_"
           << field.get_name() << ");"
           << std::endl;
        ss << "}" << std::endl;
    }

    std::string delegate_writer::name() {
        return "DelegateWriter";
    }
}
