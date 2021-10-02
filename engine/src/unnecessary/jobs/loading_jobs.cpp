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
        MeshData* result
    ) : path(std::move(path)), result(result) {}


    void UploadMeshJob::operator()(un::JobWorker* worker) {
        vk::Device device = renderer->getVirtualDevice();
        tinyobj::shape_t& shape = data->shapes[0];

        tinyobj::attrib_t& attrib = data->attrib;
        std::size_t numVertices = attrib.vertices.size() / 3;
        u32 stride = vertexLayout.getStride();
        u64 vBufSize = stride * numVertices;
        u8* vBuf = new u8[vBufSize];
        std::memset(vBuf, 0, vBufSize);
        float* asFloat = reinterpret_cast<float*>(vBuf);
        auto& meshIndices = shape.mesh.indices;
        size_t numIndices = meshIndices.size();
        std::vector<u16> indices(numIndices);

        for (size_t i = 0; i < numIndices; ++i) {
            indices[i] = meshIndices[i].vertex_index;
        }
        for (size_t i = 0; i < numVertices; ++i) {
            std::size_t vertexIndex = i;
            std::size_t normalIndex = i;
            std::size_t textureIndex = i;
            std::size_t offset = 0;
            std::size_t vertexBaseAddress = vertexIndex * stride;
            for (const auto& item: vertexLayout.getElements()) {
                std::size_t attributeAddress = vertexBaseAddress + offset;
                float* toCopy = nullptr;
                switch (item.getType()) {

                    case CommonVertexAttribute::ePosition:
                        toCopy = &attrib.vertices[vertexIndex * 3];
                        break;
                    case CommonVertexAttribute::eNormal:
                        if (normalIndex == -1) {
                            break;
                        }
                        toCopy = &attrib.normals[normalIndex * 3];
                        break;
                    case CommonVertexAttribute::eTexture:
                        if (textureIndex == -1) {
                            break;
                        }
                        toCopy = &attrib.texcoords[textureIndex];
                        break;
                    default:
                        throw std::runtime_error(
                            "Doesn't know how to process vertex input type"
                        );
                }

                size_t numBytes = item.getLength();
                if (toCopy != nullptr) {
                    std::memcpy(vBuf + attributeAddress, toCopy, numBytes);
                }
                offset += numBytes;
            }
        }
        u64 iBufSize = sizeof(u16) * numIndices;

        un::Buffer vertexStagingBuffer(
            *renderer,
            vk::BufferUsageFlagBits::eTransferSrc,
            vBufSize,
            true,
            vk::MemoryPropertyFlagBits::eHostVisible
        );
        vertexStagingBuffer.push(device, vBuf);
        un::Buffer indexStagingBuffer(
            *renderer,
            vk::BufferUsageFlagBits::eTransferSrc,
            iBufSize,
            true,
            vk::MemoryPropertyFlagBits::eHostVisible
        );

        indexStagingBuffer.push(device, indices.data());
        un::CommandBuffer uploadBuffer(
            *renderer,
            worker->getGraphicsResources().getCommandPool());
        uploadBuffer->begin(
            vk::CommandBufferBeginInfo(
                vk::CommandBufferUsageFlagBits::eOneTimeSubmit
            ));
        un::Buffer vertexBuf(
            *renderer,
            vk::BufferUsageFlagBits::eVertexBuffer |
            vk::BufferUsageFlagBits::eTransferDst,
            vBufSize,
            true
        );
        un::Buffer indexBuf(
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
        delete[] vBuf;
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
