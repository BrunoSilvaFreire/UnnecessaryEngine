#include <unnecessary/graphics/descriptors/descriptor_set.h>
#include "unnecessary/graphics/descriptors/descriptor_writer.h"

namespace un {
    DescriptorWriter::DescriptorWriter(
        Renderer* renderer
    ) : renderer(renderer),
        writes() {}

    DescriptorWriter::~DescriptorWriter() {
        if (writes.empty()) {
            return;
        }
        std::vector<vk::WriteDescriptorSet> vulkanWrites;
        std::vector<vk::DescriptorBufferInfo> bufferInfos;


        for (const DescriptorWriteOperation& operation : writes) {
            vk::DescriptorBufferInfo* pBufInfo = &bufferInfos.emplace_back(
                operation.buffer,
                0,
                VK_WHOLE_SIZE);
            vulkanWrites.emplace_back(
                operation.set,
                operation.binding,
                0,
                1,
                operation.type,
                nullptr,
                pBufInfo
            );
        }
        vk::Device device = renderer->getVirtualDevice();
        device.updateDescriptorSets(vulkanWrites, {});
    }

    void DescriptorWriter::updateUniformDescriptor(
        vk::DescriptorSet set,
        u32 binding,
        vk::Buffer buffer,
        vk::DescriptorType type
    ) {
        if (!set) {
            LOG(WARN) << "Invalid set provided @ binding: " << binding << ", buffer: "
                      << buffer;
            return;
        }
        writes.emplace_back(
            set,
            type,
            binding,
            buffer
        );
    }

    DescriptorWriter::DescriptorWriteOperation::DescriptorWriteOperation(
        const vk::DescriptorSet& set,
        vk::DescriptorType type,
        u32 binding,
        const vk::Buffer& buffer
    ) : set(set), binding(binding), buffer(buffer), type(type) {}

    DescriptorResource::DescriptorResource(
        const vk::DescriptorSetLayout& sharedSetLayout
    ) : sharedSetLayout(sharedSetLayout),
        type(un::DescriptorSetType::eShared) {}

    DescriptorResource::DescriptorResource(
        un::DescriptorSetLayout&& uniqueLayout
    ) : uniqueSetLayout(uniqueLayout),
        type(un::DescriptorSetType::eUnique) {

    }

    DescriptorSetType DescriptorResource::getType() const {
        return type;
    }

    const vk::DescriptorSetLayout& DescriptorResource::getSharedSetLayout() const {
        return sharedSetLayout;
    }

    const un::DescriptorSetLayout& DescriptorResource::getUniqueLayout() const {
        return uniqueSetLayout;
    }
}