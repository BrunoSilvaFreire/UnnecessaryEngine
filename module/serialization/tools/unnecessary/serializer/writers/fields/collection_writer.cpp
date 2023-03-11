#include "collection_writer.h"
#include "unnecessary/logging.h"

namespace un {
    bool
    collection_writer::accepts(
        const cxx_field& field,
        const cxx_translation_unit& unit,
        float& outPriority
    ) {
        outPriority = 1.5F;
        return field.get_type().get_name() == "std::vector";
    }

    void collection_writer::write_serializer(
        std::stringstream& ss,
        const cxx_field& field,
        const cxx_translation_unit& unit,
        const writer_registry& registry
    ) {
    }

    void collection_writer::write_deserializer(
        std::stringstream& ss,
        const cxx_field& field,
        const cxx_translation_unit& unit,
        const writer_registry& registry
    ) {
        const cxx_type& fieldType = field.get_type();
        if (fieldType.get_name() != "std::vector") {
            LOG(INFO) << "Given field " << fieldType.get_name() << " is not std::vector.";
            return;
        }
        std::shared_ptr<cxx_composite> ptr;
        const std::string& typeName = fieldType.get_template_types()[0];

        if (typeName != "std::string" && !unit.find_symbol(typeName, ptr)) {
            LOG(INFO) << "Unable to find composite type for vector template " << typeName
                      << " of field type "
                      << fieldType.get_name() << ".";
            return;
        }
        const std::string& fName = field.get_name();
        ss << "un::Serialized serialized_" << fName << ";" << std::endl;
        ss << "std::vector<" << typeName << "> vec_" << fName << ";" << std::endl;
        ss << "if (from.try_get(\"" << fName << "\", serialized_" << fName << ")) {" << std::endl;
        ss << "for (const auto& [id, element] : serialized_" << fName
           << ".get_named_properties()) {" << std::endl;
        ss << "un::Serialized& serialized_element = element->as<un::Serialized>();" << std::endl;
        ss << typeName << " item = un::serialization::deserialize_structure<" << typeName
           << ">(serialized_element);"
           << std::endl;
        ss << "vec_" << fName << ".push_back(std::move(item));" << std::endl;
        ss << "}" << std::endl;
        ss << "}" << std::endl;
        ss << "value." << fName << " = std::move(vec_" << fName << ");" << std::endl;
    }

    std::string collection_writer::name() {
        return "CollectionWriter";
    }
}
