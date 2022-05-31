//
// Created by bruno on 26/05/2022.
//

#ifndef UNNECESSARYENGINE_GENERATE_SERIALIZER_JOB_H
#define UNNECESSARYENGINE_GENERATE_SERIALIZER_JOB_H

#include <unnecessary/memory/membuf.h>
#include <unnecessary/jobs/simple_jobs.h>
#include <unnecessary/source_analysis/structures.h>
#include <unnecessary/serializer/writers/writer.h>
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

        void generateCompositeSerializer(
            std::stringstream& ss,
            const std::shared_ptr<CXXComposite>& comp
        );

    public:

        GenerateSerializerJob(
            std::shared_ptr<un::Buffer>  buffer,
            const std::shared_ptr<un::CXXDeclaration>& toGenerate,
            const un::CXXTranslationUnit* translationUnit
        );

        void operator()(WorkerType* worker) override;
    };


};


#endif //UNNECESSARYENGINE_GENERATE_SERIALIZER_JOB_H
