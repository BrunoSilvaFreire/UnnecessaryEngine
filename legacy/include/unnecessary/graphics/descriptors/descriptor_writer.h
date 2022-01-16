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
            vk::DeviceSize offset, range;

            DescriptorWriteOperation(
                const vk::DescriptorSet& set,
                vk::DescriptorType type,
                u32 binding,
                const vk::Buffer& buffer,
                vk::DeviceSize offset = 0,
                vk::DeviceSize range = VK_WHOLE_SIZE
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
            vk::DescriptorType type,
            vk::DeviceSize offset = 0,
            vk::DeviceSize range = VK_WHOLE_SIZE
        );
    };

}
#endif //UNNECESSARYENGINE_DESCRIPTOR_WRITER_H
