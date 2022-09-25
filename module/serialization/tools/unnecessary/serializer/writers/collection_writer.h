//
// Created by bruno on 10/05/2022.
//

#ifndef UNNECESSARYENGINE_COLLECTION_WRITER_H
#define UNNECESSARYENGINE_COLLECTION_WRITER_H

#include <unnecessary/serializer/writers/field_writer.h>

namespace un {
    class CollectionWriter : public un::FieldWriter {
    public:
        bool accepts(const CXXField& field, const un::CXXTranslationUnit& unit, float& outPriority) override;

        void write_serializer(
            std::stringstream& ss,
            const CXXField& field,
            const un::CXXTranslationUnit& unit,
            const un::WriterRegistry& registry
        ) override;

        void write_deserializer(
            std::stringstream& ss, const CXXField& field, const un::CXXTranslationUnit& unit,
            const un::WriterRegistry& registry
        ) override;

        std::string name() override;
    };
}


#endif //UNNECESSARYENGINE_COLLECTION_WRITER_H
