#include "identifiable_writer.h"

namespace un {
    bool identifiable_vector_writer::accepts(
        const cxx_field& field,
        const cxx_translation_unit& unit,
        float& outPriority
    ) {
        outPriority = 100;
        const cxx_type& fieldType = field.get_type();
        if (fieldType.get_name() != "std::vector") {
            return false;
        }
        std::shared_ptr<cxx_composite> ptr;
        const std::string& typeName = fieldType.get_template_types()[0];
        if (!unit.find_symbol(typeName, ptr)) {
            return false;
        }
        const std::vector<cxx_field>& fields = ptr->get_fields();

        return std::any_of(
            fields.begin(), fields.end(), [](const cxx_field& field) {
                return field.find_attribute("un::serialize")->has_argument("identifier");
            }
        );
    }

    void identifiable_vector_writer::write_serializer(
        std::stringstream& ss,
        const cxx_field& field,
        const cxx_translation_unit& unit,
        const writer_registry& registry
    ) {
        std::string identifierFieldName;
        const cxx_type& fieldType = field.get_type();
        if (fieldType.get_name() != "std::vector") {
            return;
        }
        std::shared_ptr<cxx_composite> ptr;
        const std::string& typeName = fieldType.get_template_types()[0];
        if (!unit.find_symbol(typeName, ptr)) {
            return;
        }
        const std::vector<cxx_field>& fields = ptr->get_fields();
        auto idField = std::find_if(
            fields.begin(), fields.end(), [](const cxx_field& candidate) {
                return candidate.find_attribute("un::serialize")->has_argument("identifier");
            }
        );

        const std::string& fName = field.get_name();
        ss << "un::Serialized serialized_" << fName << ";" << std::endl;
        ss << "for (const auto& element : value." << fName << ") {" << std::endl;
        ss << "un::Serialized serialized_element;" << std::endl;
        ss << "un::serialization::serialize_structure<" << typeName
           << ">(element, serialized_element);" << std::endl;
        ss << "serialized_" << fName << ".set(element." << idField->get_name()
           << ", serialized_element);" << std::endl;
        ss << "}" << std::endl;
    }

    void identifiable_vector_writer::write_deserializer(
        std::stringstream& ss,
        const cxx_field& field,
        const cxx_translation_unit& unit,
        const writer_registry& registry
    ) {
        std::string identifierFieldName;
        const cxx_type& fieldType = field.get_type();
        if (fieldType.get_name() != "std::vector") {
            return;
        }
        std::shared_ptr<cxx_composite> ptr;
        const std::string& typeName = fieldType.get_template_types()[0];
        if (!unit.find_symbol(typeName, ptr)) {
            return;
        }
        const std::vector<cxx_field>& fields = ptr->get_fields();
        auto idField = std::find_if(
            fields.begin(), fields.end(), [](const cxx_field& candidate) {
                return candidate.find_attribute("un::serialize")->has_argument("identifier");
            }
        );

        const std::string& fName = field.get_name();
        ss << "un::Serialized serialized_" << fName << ";" << std::endl;
        const std::string& idTypeName = idField->get_type().get_name();
        ss << "std::vector<" << typeName << "> vec_" << fName << ";" << std::endl;
        ss << "if (from.try_get(\"" << fName << "\", serialized_" << fName << ")) {" << std::endl;
        ss << "for (const auto& [id, element] : serialized_" << fName
           << ".get_named_properties()) {" << std::endl;
        ss << "un::Serialized& serialized_element = element->as<un::Serialized>();" << std::endl;
        ss << typeName << " item = un::serialization::deserialize_structure<" << typeName
           << ">(serialized_element);"
           << std::endl;

        ss << "item." << idField->get_name() << " = id;" << std::endl;
        ss << "}" << std::endl;
        ss << "}" << std::endl;
        ss << "value." << fName << " = std::move(vec_" << fName << ");" << std::endl;
    }

    std::string identifiable_vector_writer::name() {
        return "IdentifiableVectorWriter";
    }

    bool
    identifiable_field_writer::accepts(
        const cxx_field& field,
        const cxx_translation_unit& unit,
        float& outPriority
    ) {
        outPriority = 100;
        return false;
    }

    void identifiable_field_writer::write_serializer(
        std::stringstream& ss,
        const cxx_field& field,
        const cxx_translation_unit& unit,
        const writer_registry& registry
    ) {
    }

    void
    identifiable_field_writer::write_deserializer(
        std::stringstream& ss, const cxx_field& field,
        const cxx_translation_unit& unit, const writer_registry& registry
    ) {
    }

    std::string identifiable_field_writer::name() {
        return "IdentifiableFieldWriter";
    }
}
