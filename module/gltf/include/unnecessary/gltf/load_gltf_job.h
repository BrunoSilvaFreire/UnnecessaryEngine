#ifndef UNNECESSARYENGINE_LOAD_GLTF_JOB_H
#define UNNECESSARYENGINE_LOAD_GLTF_JOB_H

#include <tiny_gltf.h>
#include <unnecessary/jobs/workers/worker.h>
#include <unnecessary/rendering/geometry.h>
#include "unnecessary/memory/membuf.h"
#include <filesystem>

namespace un {
    enum class gltf_type {
        text,
        binary
    };

    class load_gltf_job : public un::Job<job_worker> {
    private:
        std::filesystem::path _path;
        gltf_type _type;
        un::local_geometry* _output;
    public:
        load_gltf_job(
            std::filesystem::path path,
            gltf_type type
        );

        void operator()(worker_type* worker) override;
    };
}
#endif
