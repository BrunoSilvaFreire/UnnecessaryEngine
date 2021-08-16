#ifndef UNNECESSARYENGINE_DESCRIPTOR_SET_H
#define UNNECESSARYENGINE_DESCRIPTOR_SET_H

#include <vulkan/vulkan.hpp>
#include <unnecessary/graphics/buffers/buffer.h>

namespace un {
    enum DescriptorSetType {
        /**
         * This descriptor set and it's layout can be used anywhere
         */
        eShared,
        /**
         * This descriptor set is unique to this owner, and is not expected to be
         * used anywhere else.
         */
        eUnique
    };

    class DescriptorResource {
    private:
        un::DescriptorSetType type;
        vk::DescriptorSetLayout sharedSetLayout;
        un::DescriptorSetLayout uniqueSetLayout;
    public:
        explicit DescriptorResource(const vk::DescriptorSetLayout& sharedSetLayout);

        explicit DescriptorResource(un::DescriptorSetLayout&& uniqueLayout);

        un::DescriptorSetType getType() const;

        const vk::DescriptorSetLayout& getSharedSetLayout() const;

        const un::DescriptorSetLayout& getUniqueLayout() const;
    };
}
#endif
