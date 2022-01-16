#include <unnecessary/jobs/loading_jobs.h>

#include <utility>
#include <fstream>
#include <unnecessary/graphics/buffers/command_buffer.h>

namespace un {

    void LoadObjJob::operator()(un::JobWorker* worker) {
        std::ifstream stream(path);
        std::string err;
        if (!tinyobj::LoadObj(
            &result->attrib,
            &result->shapes,
            &result->materials,
            &err,
            &stream
        )) {

            return;
        }
        LOG(INFO) << "Mesh " << path << " fully loaded";
    }

    LoadObjJob::LoadObjJob(
        std::filesystem::path path,
        ObjMeshData* result
    ) : path(std::move(path)), result(result) {}


    void UploadMeshJob::operator()(un::JobWorker* worker) {
        vk::Device device = renderer->getVirtualDevice();
        const auto& shape = data->getBuffer();

        const std::vector<u16>& indices = data->getIndices();
        std::size_t numVertices = data->getNumVertices();
        std::size_t numIndices = indices.singleSize();
        u32 stride = vertexLayout.getStride();
        u64 vBufSize = stride * numVertices;
        u64 iBufSize = sizeof(u16) * numIndices;

        un::GPUBuffer vertexStagingBuffer(
            *renderer,
            vk::BufferUsageFlagBits::eTransferSrc,
            vBufSize,
            true,
            vk::MemoryPropertyFlagBits::eHostVisible
        );
        vertexStagingBuffer.push(
            device,
            (void*) shape.data()
        );
        un::GPUBuffer indexStagingBuffer(
            *renderer,
            vk::BufferUsageFlagBits::eTransferSrc,
            iBufSize,
            true,
            vk::MemoryPropertyFlagBits::eHostVisible
        );

        indexStagingBuffer.push(
            device,
            (void*) indices.data()
        );
        un::CommandBuffer uploadBuffer(
            *renderer,
            worker->getGraphicsResources().getCommandPool());
        uploadBuffer->begin(
            vk::CommandBufferBeginInfo(
                vk::CommandBufferUsageFlagBits::eOneTimeSubmit
            ));
        un::GPUBuffer vertexBuf(
            *renderer,
            vk::BufferUsageFlagBits::eVertexBuffer |
            vk::BufferUsageFlagBits::eTransferDst,
            vBufSize,
            true
        );
        un::GPUBuffer indexBuf(
            *renderer,
            vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst,
            iBufSize,
            true
        );
        uploadBuffer->copyBuffer(
            vertexStagingBuffer.getVulkanBuffer(), vertexBuf.getVulkanBuffer(), {
                vk::BufferCopy(
                    0, 0, vBufSize
                )
            }
        );

        uploadBuffer->copyBuffer(
            indexStagingBuffer.getVulkanBuffer(), indexBuf.getVulkanBuffer(), {
                vk::BufferCopy(
                    0, 0, iBufSize
                )
            }
        );
        *result = MeshInfo(
            numIndices,
            un::BoundVertexLayout(vertexLayout, vertexLayoutBinding),
            vertexBuf,
            indexBuf
        );

        uploadBuffer->end();
        std::array<const vk::CommandBuffer, 1> buffers = {
            *uploadBuffer
        };
        renderer->getGraphics().getVulkan().submit(
            {
                vk::SubmitInfo(
                    {

                    },
                    {

                    },
                    buffers,
                    {
                    }
                )
            }
        );
        LOG(INFO) << "Mesh uploaded";
    }

    UploadMeshJob::UploadMeshJob(
        VertexLayout vertexLayout,
        u32 vertexLayoutBinding,
        Mesh* data,
        MeshInfo* result,
        Renderer* renderer
    ) : vertexLayout(std::move(vertexLayout)),
        vertexLayoutBinding(vertexLayoutBinding),
        data(data), result(result),
        renderer(renderer) {}


    void LoadModelJob::operator()(un::JobWorker* worker) {
        Assimp::Importer importer;
        const aiScene* pScene = importer.ReadFile(
            path.string(),
            aiProcess_Triangulate | aiProcess_SortByPType
        );
        if (pScene == nullptr) {
            return;
        }
        u32 stride = layout.getStride();
        results->resize(pScene->mNumMeshes);
        const auto& layoutElements = layout.getElements();
        for (std::size_t i = 0 ; i < pScene->mNumMeshes ; ++i) {
            aiMesh* pMesh = pScene->mMeshes[i];
            un::Mesh* mesh = results->operator[](i) = new un::Mesh();

            std::size_t numVertices = pMesh->mNumVertices;
            mesh->resize(numVertices, layout);
            //TODO: Dynamically figure out mesh attributes
            glm::vec3* vertices = reinterpret_cast<glm::vec3*>(pMesh->mVertices);
            glm::vec3* normals = reinterpret_cast<glm::vec3*>(pMesh->mNormals);
            glm::vec4* colors = reinterpret_cast<glm::vec4*>(pMesh->mColors);
            glm::vec3* uvs = reinterpret_cast<glm::vec3*>(pMesh->mTextureCoords);
            for (std::size_t j = 0 ; j < numVertices ; ++j) {
                size_t offset = 0;
                for (std::size_t k = 0 ; k < layoutElements.singleSize() ; ++k) {
                    const auto& element = layoutElements[k];
                    switch (element.getType()) {
                        case un::CommonVertexAttribute::eGeneric:
                            break;
                        case un::CommonVertexAttribute::ePosition:
                            mesh->write(vertices[j], j, stride, offset);
                            break;
                        case un::CommonVertexAttribute::eNormal:
                            mesh->write(normals[j], j, stride, offset);
                            break;
                        case un::CommonVertexAttribute::eColor:
                            mesh->write(colors[j], j, stride, offset);
                            break;
                        case un::CommonVertexAttribute::eTexture:
                            mesh->write(glm::vec2(uvs[j]), j, stride, offset);
                            break;
                    }
                    offset += element.getLength();
                }
            }
        }
    }

    LoadModelJob::LoadModelJob(
        const std::filesystem::path& path,
        std::vector<un::Mesh*>* results,
        const VertexLayout& layout
    ) : path(path), results(results), layout(layout) {}
}
