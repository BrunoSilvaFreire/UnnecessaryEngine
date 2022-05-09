#ifndef UNNECESSARYENGINE_DEFAULT_WRITER_H
#define UNNECESSARYENGINE_DEFAULT_WRITER_H

#include <unnecessary/serializer/writers/writer.h>

namespace un {

    class PrimitiveWriter : public un::SerializationWriter {
        bool accepts(const CXXField& field, float& outPriority) override;

        void write_serializer(std::stringstream& ss, const CXXField& field) override;

        void write_deserializer(std::stringstream& ss, const CXXField& field) override;

    public:
        std::string name() override;
    };
}
#endif
