//
// Created by bruno on 21/06/2021.
//

#ifndef UNNECESSARYENGINE_DESCRIPTOR_ALLOCATOR_H
#define UNNECESSARYENGINE_DESCRIPTOR_ALLOCATOR_H

#include <vector>
#include <stack>
#include <unnecessary/graphics/descriptor_layout.h>
#include <unnecessary/def.h>

namespace un {
    class DescriptorPool {
    private:
        vk::DescriptorPool pool;
        size_t freeSets;
        size_t numSets;

        void assertCanAllocate(size_t count) const;

    public:
        DescriptorPool(
                vk::Device device,
                un::DescriptorSetLayout &layout,
                size_t numSetsToReserve
        );

        bool isFull();

        const vk::DescriptorPool &getPool() const;

        size_t getFreeSets() const;

        size_t getNumSets() const;

        size_t getUsedSets() const;


        void allocate(
                size_t count,
                vk::DescriptorSet *setPtr,
                vk::Device device,
                vk::DescriptorSetLayout layout
        );
    };

    class DescriptorAllocator {
    private:
        std::stack<un::DescriptorPool> freePools;
        un::DescriptorSetLayout layout;
        vk::DescriptorSetLayout vulkanLayout;
        /**
         * How many sets each pool holds
         */
        size_t poolReserveCount;
        vk::Device owningDevice;
        un::DescriptorPool create();

    public:
        DescriptorAllocator(
                DescriptorSetLayout&& oldLayout,
                vk::Device owningDevice,
                size_t numSetsPerPool,
                vk::ShaderStageFlagBits shaderStageFlags
        );

        ~DescriptorAllocator();

        vk::DescriptorSet allocate();

        std::vector<vk::DescriptorSet> allocate(size_t count);
    };
}
#endif //UNNECESSARYENGINE_DESCRIPTOR_ALLOCATOR_H
