//
// Created by bruno on 26/05/2022.
//

#ifndef UNNECESSARYENGINE_GENERATE_SERIALIZER_JOB_H
#define UNNECESSARYENGINE_GENERATE_SERIALIZER_JOB_H

#include <unnecessary/memory/membuf.h>
#include <unnecessary/jobs/simple_jobs.h>
#include <unnecessary/source_analysis/structures.h>
#include <unnecessary/serializer/writers/fields/field_writer.h>
#include <unnecessary/serializer/generation.h>

namespace un {
    class generate_serializer_job : public simple_job {
    private:
        generation_info _info;
        std::shared_ptr<byte_buffer> _buffer;
        std::shared_ptr<cxx_declaration> _toGenerate;
        const cxx_translation_unit* _translationUnit;
        writer_registry _writerRegistry;

        void
        generate_info_comments(std::stringstream& ss, const std::shared_ptr<cxx_composite>& ptr);

        void write_field_info(
            std::stringstream& stream,
            const cxx_field& field,
            const std::shared_ptr<field_writer>& writer
        );

        void serialize_fields(
            const std::shared_ptr<cxx_composite>& composite,
            std::stringstream& ss
        );

        void generate_composite_serializer(
            std::stringstream& ss,
            const std::shared_ptr<cxx_composite>& comp
        );

    public:
        generate_serializer_job(
            std::shared_ptr<byte_buffer> buffer,
            const std::shared_ptr<cxx_declaration>& toGenerate,
            const cxx_translation_unit* translationUnit
        );

        void operator()(worker_type* worker) override;
    };
};

#endif //UNNECESSARYENGINE_GENERATE_SERIALIZER_JOB_H
