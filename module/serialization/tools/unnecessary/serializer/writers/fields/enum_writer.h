//
// Created by brunorbsf on 25/09/22.
//

#ifndef UNNECESSARYENGINE_ENUM_WRITER_H
#define UNNECESSARYENGINE_ENUM_WRITER_H


#include "field_writer.h"

namespace un {
    class EnumByNameWriter : public un::FieldWriter {
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
