#ifndef UNNECESSARYENGINE_DEFAULT_WRITER_H
#define UNNECESSARYENGINE_DEFAULT_WRITER_H

#include <unnecessary/serializer/writers/field_writer.h>

namespace un {

    class PrimitiveWriter : public un::FieldWriter {
        bool accepts(const CXXField& field, const un::CXXTranslationUnit& unit, float& outPriority) override;

        void write_serializer(std::stringstream& ss, const CXXField& field, const un::CXXTranslationUnit& unit,
                              const un::WriterRegistry& registry) override;

        void write_deserializer(std::stringstream& ss, const CXXField& field, const un::CXXTranslationUnit& unit,
                                const un::WriterRegistry& registry) override;

    public:
        std::string name() override;
    };
}
#endif
