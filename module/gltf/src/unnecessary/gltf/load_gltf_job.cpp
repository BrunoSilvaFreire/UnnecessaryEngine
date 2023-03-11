#include <filesystem>
#include <unnecessary/gltf/load_gltf_job.h>
#include <unnecessary/jobs/workers/worker.h>

namespace un {
    void LoadGLTFJob::operator()(un::job_worker* worker) {
        tinygltf::TinyGLTF loader;
        tinygltf::Model model;
        std::string err;
        std::string warn;
        switch (_type) {
            case GLTFType::eText:
                loader.LoadASCIIFromFile(
                    &model,
                    &err,
                    &warn,
                    _path.string()
                );
                break;
            case GLTFType::eBinary:
                loader.LoadBinaryFromFile(
                    &model,
                    &err,
                    &warn,
                    _path.string()
                );
                break;
        }
        _output = new un::LocalGeometry();
        for (const auto& buf : model._buffers) {

        }
    }

    LoadGLTFJob::LoadGLTFJob(
        std::filesystem::path path,
        GLTFType type
    ) : _path(path), _type(type) {
    }

}
