//
// Created by bruno on 12/08/2021.
//

#ifndef UNNECESSARYENGINE_DESCRIPTOR_WRITER_H
#define UNNECESSARYENGINE_DESCRIPTOR_WRITER_H

#include <vulkan/vulkan.hpp>
#include <unnecessary/graphics/buffers/buffer.h>

namespace un {
    class DescriptorWriter {
    private:
        struct DescriptorWriteOperation {
            vk::DescriptorSet set;
            vk::DescriptorType type;
            u32 binding{};
            vk::Buffer buffer;

            DescriptorWriteOperation(
                const vk::DescriptorSet& set,
                vk::DescriptorType type,
                u32 binding,
                const vk::Buffer& buffer
            );
        };

        std::vector<DescriptorWriteOperation> writes;
        un::Renderer* renderer;
    public:
        DescriptorWriter(un::Renderer* renderer);

        ~DescriptorWriter();

        void updateUniformDescriptor(
            vk::DescriptorSet set,
            u32 binding,
            vk::Buffer buffer,
            vk::DescriptorType type
        );
    };

}
#endif //UNNECESSARYENGINE_DESCRIPTOR_WRITER_H
