#include <unnecessary/serializer/writers/writer.h>
#include <unnecessary/serializer/writers/complex_writer.h>
#include <unnecessary/serializer/writers/default_writer.h>
#include <unnecessary/serializer/writers/delegate_writer.h>
#include <unnecessary/serializer/writers/identifiable_writer.h>
#include <unnecessary/serializer/writers/collection_writer.h>
#include <iostream>

namespace un {
    bool SerializationWriter::isOptional(const CXXField& field) {
        return field.findAttribute("un::serialize")->hasArgument("optional");
    }

    void SerializationWriter::addMissingFieldException(std::stringstream& ss, const CXXField& field) {
        addMissingFieldException(ss, field.getName());
    }

    void SerializationWriter::addMissingFieldException(std::stringstream& ss, const std::string& fieldName) {
        ss << "throw std::runtime_error(\"Unable to read field " << fieldName << "\");" << std::endl;
    }


    bool SerializationWriter::trySerializePrimitive(
        std::stringstream& ss,
        const std::string& name,
        const CXXType& primitiveCandidate
    ) {
        if (primitiveCandidate.getName() != "std::string") {
            if (primitiveCandidate.getKind() == un::CXXTypeKind::eComplex) {
                return false;
            }
        }

        ss << "into.set<" << primitiveCandidate.getName() << ">" << "(\"" << name << "\", value." << name << ");"
           << std::endl;
        return true;
    }

    bool SerializationWriter::tryDeserializePrimitive(
        std::stringstream& ss,
        const std::string& name,
        const CXXType& primitiveCandidate
    ) {
        if (primitiveCandidate.getName() != "std::string") {
            if (primitiveCandidate.getKind() == un::CXXTypeKind::eComplex) {
                return false;
            }
        }
        std::string typeName = primitiveCandidate.getName();

        ss << "if (!from.try_get<" << typeName << ">"
           << "(\"" << name << "\", value." << name << ")) {" << std::endl;
        addMissingFieldException(ss, name);
        ss << "}" << std::endl;
        return true;
    }


    std::shared_ptr<un::SerializationWriter> WriterRegistry::getWriter(
        const un::CXXField& field,
        const un::CXXTranslationUnit& unit
    ) const {
        const auto& bestWriter = std::max_element(
            writers.begin(), writers.end(),
            [&](
                const std::shared_ptr<un::SerializationWriter>& a,
                const std::shared_ptr<un::SerializationWriter>& b
            ) {
                float first;
                float second;
                if (!a->accepts(field, unit, first)) {
                    return true;
                }
                if (!b->accepts(field, unit, second)) {
                    return false;
                }
                return first < second;
            }
        );
        if (bestWriter == writers.end()) {
            throw std::runtime_error("No suitable writer found");
        }
        return *bestWriter;
    }

    WriterRegistry::WriterRegistry() : writers() {
        addWriter<un::PrimitiveWriter>();
        addWriter<un::ComplexWriter>();
        addWriter<un::DelegateWriter>();
        addWriter<un::IdentifiableVectorWriter>();
        addWriter<un::CollectionWriter>();
    }

}