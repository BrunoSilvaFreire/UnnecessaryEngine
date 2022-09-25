//
// Created by bruno on 09/05/2022.
//

#ifndef UNNECESSARYENGINE_DELEGATE_WRITER_H
#define UNNECESSARYENGINE_DELEGATE_WRITER_H

#include "field_writer.h"

namespace un {
    class DelegateWriter : public un::FieldWriter {
    public:
        bool accepts(const CXXField& field, const un::CXXTranslationUnit& unit, float& outPriority) override;

        void write_serializer(std::stringstream& ss, const CXXField& field, const un::CXXTranslationUnit& unit,
                              const un::WriterRegistry& registry) override;

        void write_deserializer(std::stringstream& ss, const CXXField& field, const un::CXXTranslationUnit& unit,
                                const un::WriterRegistry& registry) override;

        std::string name() override;
    };
}

#endif //UNNECESSARYENGINE_DELEGATE_WRITER_H
