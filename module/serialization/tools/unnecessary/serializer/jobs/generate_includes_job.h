//
// Created by bruno on 03/06/2022.
//

#ifndef UNNECESSARYENGINE_GENERATE_INCLUDES_JOB_H
#define UNNECESSARYENGINE_GENERATE_INCLUDES_JOB_H

#include <unnecessary/memory/membuf.h>
#include <unnecessary/jobs/simple_jobs.h>
#include <unnecessary/source_analysis/structures.h>
#include <unnecessary/serializer/generation_plan.h>

namespace un {
    class generate_includes_jobs : public simple_job {
    private:
        const_ptr<cxx_translation_unit> _translationUnit;
        const_ptr<generation_plan> _plan;
        u32 _unitGraphIndex;
        std::shared_ptr<byte_buffer> _buf;

    public:
        generate_includes_jobs(
            u32 unitGraphIndex,
            const_ptr<cxx_translation_unit> translationUnit,
            const_ptr<generation_plan> plan,
            std::shared_ptr<byte_buffer> buf
        );

        void operator()(worker_type* worker) override;
    };
}

#endif //UNNECESSARYENGINE_GENERATE_INCLUDES_JOB_H
