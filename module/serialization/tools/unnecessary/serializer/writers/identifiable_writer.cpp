#include <unnecessary/serializer/writers/identifiable_writer.h>

namespace un {

    bool IdentifiableVectorWriter::accepts(
        const CXXField& field,
        const un::CXXTranslationUnit& unit,
        float& outPriority
    ) {
        outPriority = 100;
        const un::CXXType& fieldType = field.getType();
        if (fieldType.getName() != "std::vector") {
            return false;
        }
        std::shared_ptr<un::CXXComposite> ptr;
        const std::string& typeName = fieldType.getTemplateTypes()[0];
        if (!unit.findSymbol(typeName, ptr)) {
            return false;
        }
        const std::vector<CXXField>& fields = ptr->getFields();


        return std::any_of(fields.begin(), fields.end(), [](const un::CXXField& field) {
            return field.findAttribute("un::serialize")->hasArgument("identifier");
        });
    }

    void IdentifiableVectorWriter::write_serializer(
        std::stringstream& ss,
        const CXXField& field,
        const un::CXXTranslationUnit& unit,
        const un::WriterRegistry& registry
    ) {
        std::string identifierFieldName;
        const un::CXXType& fieldType = field.getType();
        if (fieldType.getName() != "std::vector") {
            return;
        }
        std::shared_ptr<un::CXXComposite> ptr;
        const std::string& typeName = fieldType.getTemplateTypes()[0];
        if (!unit.findSymbol(typeName, ptr)) {
            return;
        }
        const std::vector<CXXField>& fields = ptr->getFields();
        auto idField = std::find_if(fields.begin(), fields.end(), [](const un::CXXField& candidate) {
            return candidate.findAttribute("un::serialize")->hasArgument("identifier");
        });

        const std::string& fName = field.getName();
        ss << "un::Serialized serialized_" << fName << ";" << std::endl;
        ss << "for (const auto& element : value." << fName << ") {" << std::endl;
        ss << "un::Serialized serialized_element;" << std::endl;
        ss << "un::serialization::serialize_structure<" << typeName << ">(element, serialized_element);" << std::endl;
        ss << "serialized_" << fName << ".set(element." << idField->getName() << ", serialized_element);" << std::endl;
        ss << "}" << std::endl;
    }

    void IdentifiableVectorWriter::write_deserializer(
        std::stringstream& ss,
        const CXXField& field,
        const un::CXXTranslationUnit& unit,
        const un::WriterRegistry& registry
    ) {
        std::string identifierFieldName;
        const un::CXXType& fieldType = field.getType();
        if (fieldType.getName() != "std::vector") {
            return;
        }
        std::shared_ptr<un::CXXComposite> ptr;
        const std::string& typeName = fieldType.getTemplateTypes()[0];
        if (!unit.findSymbol(typeName, ptr)) {
            return;
        }
        const std::vector<CXXField>& fields = ptr->getFields();
        auto idField = std::find_if(fields.begin(), fields.end(), [](const un::CXXField& candidate) {
            return candidate.findAttribute("un::serialize")->hasArgument("identifier");
        });

        const std::string& fName = field.getName();
        ss << "un::Serialized serialized_" << fName << ";" << std::endl;
        const std::string& idTypeName = idField->getType().getName();
        ss << "std::vector<" << typeName << "> vec_" << fName << ";" << std::endl;
        ss << "if (from.try_get(\"" << fName << "\", serialized_" << fName << ")) {" << std::endl;
        ss << "for (const auto& [id, element] : serialized_" << fName << ".getNamedProperties()) {" << std::endl;
        ss << "un::Serialized& serialized_element = element->as<un::Serialized>();" << std::endl;
        ss << typeName << " item = un::serialization::deserialize_structure<" << typeName << ">(serialized_element);"
           << std::endl;

        ss << "item." << idField->getName() << " = id;" << std::endl;
        ss << "}" << std::endl;
        ss << "}" << std::endl;
        ss << "value." << fName << " = std::move(vec_" << fName << ");" << std::endl;
    }

    std::string IdentifiableVectorWriter::name() {
        return "IdentifiableVectorWriter";
    }

    bool IdentifiableFieldWriter::accepts(const CXXField& field, const CXXTranslationUnit& unit, float& outPriority) {
        outPriority = 100;
        return false;
    }

    void IdentifiableFieldWriter::write_serializer(
        std::stringstream& ss,
        const CXXField& field,
        const CXXTranslationUnit& unit,
        const WriterRegistry& registry
    ) {

    }

    void IdentifiableFieldWriter::write_deserializer(std::stringstream& ss, const CXXField& field,
                                                     const CXXTranslationUnit& unit, const WriterRegistry& registry) {

    }

    std::string IdentifiableFieldWriter::name() {
        return "IdentifiableFieldWriter";
    }
}