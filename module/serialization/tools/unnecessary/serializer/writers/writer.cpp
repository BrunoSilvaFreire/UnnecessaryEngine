#include <unnecessary/serializer/writers/writer.h>
#include <unnecessary/serializer/writers/complex_writer.h>
#include <unnecessary/serializer/writers/default_writer.h>
#include <unnecessary/serializer/writers/delegate_writer.h>
#include <iostream>

namespace un {
    bool SerializationWriter::isOptional(const CXXField& field) {
        return field.findAttribute("un::serialize")->hasArgument("optional");
    }

    void SerializationWriter::addMissingFieldException(std::stringstream& ss, const CXXField& field) {
        ss << "throw std::runtime_error(\"Unable to read field " << field.getName() << "\");" << std::endl;
    }

    std::shared_ptr<un::SerializationWriter> WriterRegistry::getWriter(const CXXField& field) {
        return *std::max_element(
            writers.begin(), writers.end(),
            [&](const std::shared_ptr<un::SerializationWriter>& a,
                const std::shared_ptr<un::SerializationWriter>& b
            ) {
                float first;
                float second;
                if (!a->accepts(field, first)) {
                    return true;
                }
                if (!b->accepts(field, second)) {
                    return false;
                }
                return first < second;
            }
        );
    }

    WriterRegistry::WriterRegistry() : writers() {
        addWriter<un::PrimitiveWriter>();
        addWriter<un::ComplexWriter>();
        addWriter<un::DelegateWriter>();
    }

}