#include <unnecessary/serializer/writers/default_writer.h>

namespace un {

    bool PrimitiveWriter::accepts(const CXXField& field, const un::CXXTranslationUnit& unit, float& outPriority) {
        if (field.getType().getName() == "std::string") {
            outPriority = 1.5F; //Overtake complex writer
        } else {
            outPriority = 0;

        }
        return true;
    }

    void PrimitiveWriter::write_deserializer(std::stringstream& ss, const CXXField& field,
                                             const un::CXXTranslationUnit& unit,
                                             const un::WriterRegistry& registry) {
        std::string fName = field.getName();
        std::string typeName = field.getType().getName();
        bool optional = isOptional(field);
        if (optional) {
            ss << "from.try_get<" << typeName << ">" << "(\"" << fName << "\", value." << fName << "); // Optional"
               << std::endl;
        } else {
            ss << "if (!from.try_get<" << typeName << ">"
               << "(\"" << fName << "\", value." << fName << ")) {" << std::endl;
            addMissingFieldException(ss, field);
            ss << "}" << std::endl;
        }
    }

    void
    PrimitiveWriter::write_serializer(std::stringstream& ss, const CXXField& field, const un::CXXTranslationUnit& unit,
                                      const un::WriterRegistry& registry) {
        ss << "into.set<" << field.getType().getName() << ">"
           << "(\"" << field.getName() << "\", value." << field.getName() << ");" << std::endl;

    }

    std::string PrimitiveWriter::name() {
        return "PrimitiveWriter";
    }


}