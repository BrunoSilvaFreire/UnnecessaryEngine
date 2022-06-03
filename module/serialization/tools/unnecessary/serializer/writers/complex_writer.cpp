#include <unnecessary/serializer/writers/complex_writer.h>

namespace un {

    bool ComplexWriter::accepts(const CXXField& field, const un::CXXTranslationUnit& unit, float& outPriority) {
        if (field.getType().getName() == "std::string") {
            return false;
        }
        outPriority = 1.0F;
        return field.getType().getKind() == un::CXXTypeKind::eComplex;
    }

    void ComplexWriter::write_serializer(
        std::stringstream& ss, const CXXField& field, const un::CXXTranslationUnit& unit,
        const un::WriterRegistry& registry
    ) {
        std::string fName = field.getName();
        std::string typeName = field.getType().getName();
        bool optional = isOptional(field);
        ss << "un::Serialized serialized_" << fName << ";" << std::endl;
        ss << "un::serialization::serialize<" << typeName << ">("
           << "value." << field.getName() << ", " << "serialized_" << fName
           << ");" << std::endl;

        ss << "into.set<un::Serialized>(\"" << fName << "\", serialized_" << fName << ");" << std::endl;
    }

    void
    ComplexWriter::write_deserializer(
        std::stringstream& ss, const CXXField& field, const un::CXXTranslationUnit& unit,
        const un::WriterRegistry& registry
    ) {
        std::string fName = field.getName();
        std::string typeName = field.getType().getName();
        bool optional = isOptional(field);
        ss << "un::Serialized serialized_" << fName << ";" << std::endl;
        if (optional) {
            ss << "if (from.try_get(\"" << fName << "\", serialized_" << fName << ")) {" << std::endl;
            ss << "value." << fName << " = un::serialization::deserialize<" << typeName << ">(serialized_" << fName
               << ");" << std::endl;
            ss << "}" << std::endl;
        } else {
            ss << "if (!from.try_get(\"" << fName << "\", serialized_"
               << fName << ")) {" << std::endl;
            ss << "throw std::runtime_error(\"Unable to read field " << fName << "\");"
               << std::endl;
            ss << "}" << std::endl;
            ss << "value." << fName << " = un::serialization::deserialize<"
               << typeName << ">(serialized_" << fName << ");" << std::endl;
        }
    }

    std::string ComplexWriter::name() {
        return "ComplexWriter";
    }
}