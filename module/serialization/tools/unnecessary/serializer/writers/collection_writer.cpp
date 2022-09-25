#include <unnecessary/serializer/writers/collection_writer.h>
#include <unnecessary/logging.h>

namespace un {

    bool CollectionWriter::accepts(const CXXField& field, const un::CXXTranslationUnit& unit, float& outPriority) {
        outPriority = 1.5F;
        return field.getType().getName() == "std::vector";
    }

    void CollectionWriter::write_serializer(
        std::stringstream& ss,
        const CXXField& field,
        const un::CXXTranslationUnit& unit,
        const un::WriterRegistry& registry
    ) {

    }

    void CollectionWriter::write_deserializer(
        std::stringstream& ss,
        const CXXField& field,
        const un::CXXTranslationUnit& unit,
        const un::WriterRegistry& registry
    ) {
        const un::CXXType& fieldType = field.getType();
        if (fieldType.getName() != "std::vector") {
            LOG(INFO) << "Given field " << fieldType.getName() << " is not std::vector.";
            return;
        }
        std::shared_ptr<un::CXXComposite> ptr;
        const std::string& typeName = fieldType.getTemplateTypes()[0];

        if (typeName != "std::string" && !unit.findSymbol(typeName, ptr)) {
            LOG(INFO) << "Unable to find composite type for vector template " << typeName << " of field type "
                      << fieldType.getName() << ".";
            return;
        }
        const std::string& fName = field.getName();
        ss << "un::Serialized serialized_" << fName << ";" << std::endl;
        ss << "std::vector<" << typeName << "> vec_" << fName << ";" << std::endl;
        ss << "if (from.try_get(\"" << fName << "\", serialized_" << fName << ")) {" << std::endl;
        ss << "for (const auto& [id, element] : serialized_" << fName << ".getNamedProperties()) {" << std::endl;
        ss << "un::Serialized& serialized_element = element->as<un::Serialized>();" << std::endl;
        ss << typeName << " item = un::serialization::deserialize_structure<" << typeName << ">(serialized_element);"
           << std::endl;
        ss << "vec_" << fName << ".push_back(std::move(item));" << std::endl;
        ss << "}" << std::endl;
        ss << "}" << std::endl;
        ss << "value." << fName << " = std::move(vec_" << fName << ");" << std::endl;

    }

    std::string CollectionWriter::name() {
        return "CollectionWriter";
    }
}
