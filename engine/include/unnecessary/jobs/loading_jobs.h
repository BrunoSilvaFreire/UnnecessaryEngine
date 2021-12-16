//
// Created by bruno on 18/01/2021.
//

#ifndef UNNECESSARYENGINE_LOADING_JOBS_H
#define UNNECESSARYENGINE_LOADING_JOBS_H

#include <unnecessary/jobs/jobs.h>
#include <unnecessary/graphics/mesh.h>
#include <unnecessary/graphics/renderer.h>
#include <filesystem>
#include <tiny_obj_loader.h>
#include <vulkan/vulkan.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace un {
    class LoadObjJob : public Job {
    private:
        std::filesystem::path path;
        WRITE_ONLY()
        ObjMeshData* result;
    public:
        LoadObjJob(std::filesystem::path path, ObjMeshData* result);

        void operator()(un::JobWorker* worker) override;

    };

    struct PosNorVertex {
        glm::vec3 position;
        glm::vec3 normal;
    };

    class LoadModelJob : public Job {
    private:
        std::filesystem::path path;
        WRITE_ONLY()
        std::vector<un::Mesh*>* results;
        un::VertexLayout layout;
    public:
        LoadModelJob(
            const std::filesystem::path& path,
            std::vector<un::Mesh*>* results,
            const VertexLayout& layout
        );

        void operator()(un::JobWorker* worker) override;
    };

    class UploadMeshJob : public Job {
    private:
        un::VertexLayout vertexLayout;
        u32 vertexLayoutBinding;

        READ_ONLY()
        Mesh* data;

        WRITE_ONLY()
        MeshInfo* result;
        un::Renderer* renderer;
    public:
        UploadMeshJob(
            VertexLayout vertexLayout,
            u32 vertexLayoutBinding,
            Mesh* data,
            MeshInfo* result,
            Renderer* renderer
        );

        void operator()(un::JobWorker* worker) override;

    };
}

#endif //UNNECESSARYENGINE_LOADING_JOBS_H
