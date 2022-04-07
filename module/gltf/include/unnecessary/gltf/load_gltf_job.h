#ifndef UNNECESSARYENGINE_LOAD_GLTF_JOB_H
#define UNNECESSARYENGINE_LOAD_GLTF_JOB_H

#include <tiny_gltf.h>
#include <unnecessary/jobs/workers/worker.h>
#include <unnecessary/rendering/geometry.h>
#include "unnecessary/memory/membuf.h"
#include <filesystem>

namespace un {
    enum class GLTFType {
        eText,
        eBinary
    };

    class LoadGLTFJob : public un::SimpleJob {
    private:
        std::filesystem::path _path;
        GLTFType _type;
        un::LocalGeometry* _output;
    public:
        LoadGLTFJob(
            std::filesystem::path path,
            GLTFType type
        );

        void operator()(WorkerType* worker) override;
    };
}
#endif
