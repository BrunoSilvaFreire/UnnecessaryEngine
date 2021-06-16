#include <unnecessary/jobs/loading_jobs.h>

#include <utility>
#include <fstream>

namespace un {

    void LoadObjJob::operator()(un::JobWorker *worker) {
        std::ifstream stream(path);
        std::string err;
        if (!tinyobj::LoadObj(&result->attrib, &result->shapes, &result->materials, &err, &stream)) {
            return;
        }
        LOG(INFO) << "Mesh " << path << " fully loaded";
    }

    LoadObjJob::LoadObjJob(
            std::filesystem::path path,
            MeshData *result
    ) : path(std::move(path)), result(result) {}


    void UploadMeshJob::operator()(un::JobWorker *worker) {
        vk::Device device = renderer->getVirtualDevice();
        tinyobj::shape_t &shape = data->shapes[0];

        u64 vBufSize = vertexLayout.getStride() * data->attrib.vertices.size();
        u64 iBufSize = sizeof(u16) * shape.mesh.indices.size();

        un::Buffer stagingBuf(
                *renderer,
                vk::BufferUsageFlagBits::eTransferSrc,
                vBufSize,
                true
        );
        un::CommandBuffer uploadBuffer(*renderer, worker->getCommandBufferPool());
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
        buf.copyBuffer(stagingBuf.getVulkanBuffer(), vertexBuf.getVulkanBuffer(), {
                vk::BufferCopy(
                        0, 0, vBufSize
                )
        });
        un::Buffer indexBuf(*renderer, vk::BufferUsageFlagBits::eIndexBuffer, iBufSize, true);
        *result = MeshInfo(
                shape.mesh.indices.size(),
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

                                }, {

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
            MeshData *data,
            MeshInfo *result,
            Renderer *renderer
    ) : vertexLayout(std::move(vertexLayout)),
        vertexLayoutBinding(vertexLayoutBinding),
        data(data), result(result),
        renderer(renderer) {}
}
