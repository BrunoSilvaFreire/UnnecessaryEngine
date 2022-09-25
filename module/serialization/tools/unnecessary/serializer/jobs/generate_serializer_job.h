//
// Created by bruno on 26/05/2022.
//

#ifndef UNNECESSARYENGINE_GENERATE_SERIALIZER_JOB_H
#define UNNECESSARYENGINE_GENERATE_SERIALIZER_JOB_H

#include <unnecessary/memory/membuf.h>
#include <unnecessary/jobs/simple_jobs.h>
#include <unnecessary/source_analysis/structures.h>
#include "unnecessary/serializer/writers/fields/field_writer.h"
#include <unnecessary/serializer/generation.h>

namespace un {
    class GenerateSerializerJob : public un::SimpleJob {
    private:
        un::GenerationInfo info;
        std::shared_ptr<un::Buffer> buffer;
        std::shared_ptr<un::CXXDeclaration> toGenerate;
        const un::CXXTranslationUnit* translationUnit;
        un::WriterRegistry writerRegistry;

        void generateInfoComments(std::stringstream& ss, const std::shared_ptr<CXXComposite>& ptr);

        void writeFieldInfo(
            std::stringstream& stream,
            const CXXField& field,
            const std::shared_ptr<FieldWriter>& writer
        );

        void serializeFields(
            const std::shared_ptr<un::CXXComposite>& composite,
            std::stringstream& ss
        );

        void generateCompositeSerializer(
            std::stringstream& ss,
            const std::shared_ptr<CXXComposite>& comp
        );

    public:

        GenerateSerializerJob(
            std::shared_ptr<un::Buffer> buffer,
            const std::shared_ptr<un::CXXDeclaration>& toGenerate,
            const un::CXXTranslationUnit* translationUnit
        );

        void operator()(WorkerType* worker) override;

        void addStaticRediction(std::stringstream& ss) const;
    };


};


#endif //UNNECESSARYENGINE_GENERATE_SERIALIZER_JOB_H
