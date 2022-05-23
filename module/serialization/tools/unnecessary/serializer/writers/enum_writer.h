//
// Created by bruno on 18/05/2022.
//

#ifndef UNNECESSARYENGINE_ENUM_WRITER_H
#define UNNECESSARYENGINE_ENUM_WRITER_H

#include <unnecessary/serializer/writers/writer.h>

namespace un {
    class EnumWriter : public un::SerializationWriter {
    public:
        bool accepts(const CXXField& field, const CXXTranslationUnit& unit, float& outPriority) override;

        void write_serializer(
            std::stringstream& ss,
            const CXXField& field,
            const CXXTranslationUnit& unit,
            const WriterRegistry& registry
        ) override;

        void write_deserializer(
            std::stringstream& ss,
            const CXXField& field,
            const CXXTranslationUnit& unit,
            const WriterRegistry& registry
        ) override;

        std::string name() override;
    };
}


#endif //UNNECESSARYENGINE_ENUM_WRITER_H
