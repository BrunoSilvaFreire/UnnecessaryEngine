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

namespace un {
    class LoadObjJob : public Job {
    private:
        std::filesystem::path path;
        WRITE_ONLY()
        MeshData *result;
    public:
        LoadObjJob(std::filesystem::path path, MeshData *result);

        explicit operator void() override;

    };

    class UploadMeshJob : public Job {
    private:
        un::VertexLayout vertexLayout;
        u32 vertexLayoutBinding;

        READ_ONLY()
        MeshData *data;

        WRITE_ONLY()
        MeshInfo *result;
        un::Renderer *renderer;
    public:
        UploadMeshJob(
                VertexLayout vertexLayout,
                u32 vertexLayoutBinding,
                MeshData *data,
                MeshInfo *result,
                Renderer *renderer
        );

        explicit operator void() override;

    };
}

#endif //UNNECESSARYENGINE_LOADING_JOBS_H
