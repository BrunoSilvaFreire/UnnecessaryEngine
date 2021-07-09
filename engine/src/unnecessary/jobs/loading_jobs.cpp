#include <unnecessary/jobs/loading_jobs.h>

#include <utility>
#include <fstream>

namespace un {

    void LoadObjJob::operator()(un::JobWorker* worker) {
        std::ifstream stream(path);
        std::string err;
        if (!tinyobj::LoadObj(&result->attrib, &result->shapes, &result->materials, &err, &stream)) {
            return;
        }
        LOG(INFO) << "Mesh " << path << " fully loaded";
    }

    LoadObjJob::LoadObjJob(
            std::filesystem::path path,
            MeshData* result
    ) : path(std::move(path)), result(result) {}


    void UploadMeshJob::operator()(un::JobWorker* worker) {
        vk::Device device = renderer->getVirtualDevice();
        tinyobj::shape_t& shape = data->shapes[0];

        tinyobj::attrib_t& attrib = data->attrib;
        u64 vBufSize = vertexLayout.getStride() * (attrib.vertices.size() / 3);
        auto& meshIndices = shape.mesh.indices;
        size_t numIndices = meshIndices.size();
        std::vector<u16> indices(numIndices);
        for (size_t i = 0; i < numIndices; ++i) {
            indices[i] = meshIndices[i].vertex_index;
        }
        u64 iBufSize = sizeof(u16) * numIndices;

        un::Buffer vertexStagingBuffer(
                *renderer,
                vk::BufferUsageFlagBits::eTransferSrc,
                vBufSize,
                true,
                vk::MemoryPropertyFlagBits::eHostVisible
        );
        auto& vertices = attrib.vertices;
        vertexStagingBuffer.push(device, vertices.data());
        un::Buffer indexStagingBuffer(
                *renderer,
                vk::BufferUsageFlagBits::eTransferSrc,
                iBufSize,
                true,
                vk::MemoryPropertyFlagBits::eHostVisible
        );
        indexStagingBuffer.push(device, indices.data());
        un::CommandBuffer uploadBuffer(*renderer, worker->getGraphicsResources().getCommandPool());
        vk::CommandBuffer buf = uploadBuffer.getVulkanBuffer();
        buf.begin(vk::CommandBufferBeginInfo(
                vk::CommandBufferUsageFlagBits::eOneTimeSubmit
        ));
        un::Buffer vertexBuf(
                *renderer,
                vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
                vBufSize,
                true
        );
        un::Buffer indexBuf(
                *renderer,
                vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst,
                iBufSize,
                true
        );
        buf.copyBuffer(vertexStagingBuffer.getVulkanBuffer(), vertexBuf.getVulkanBuffer(), {
                vk::BufferCopy(
                        0, 0, vBufSize
                )
        });

        buf.copyBuffer(indexStagingBuffer.getVulkanBuffer(), indexBuf.getVulkanBuffer(), {
                vk::BufferCopy(
                        0, 0, iBufSize
                )
        });
        *result = MeshInfo(
                numIndices,
                un::BoundVertexLayout(vertexLayout, vertexLayoutBinding),
                vertexBuf,
                indexBuf
        );

        buf.end();
        std::array<const vk::CommandBuffer, 1> buffers = {
                buf
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
            MeshData* data,
            MeshInfo* result,
            Renderer* renderer
    ) : vertexLayout(std::move(vertexLayout)),
        vertexLayoutBinding(vertexLayoutBinding),
        data(data), result(result),
        renderer(renderer) {}
}
