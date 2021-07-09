#include <unnecessary/graphics/descriptor_allocator.h>

namespace un {

    DescriptorPool::DescriptorPool(vk::Device device, DescriptorLayout& layout, size_t numSetsToReserve) : numSets(
            numSetsToReserve), freeSets(numSetsToReserve) {
        std::vector<vk::DescriptorPoolSize> descriptorSizes;
        for (un::DescriptorElement& element : layout.getElements()) {
            descriptorSizes.emplace_back(
                    element.getType(),
                    1
            );
        }
        pool = device.createDescriptorPool(
                vk::DescriptorPoolCreateInfo(
                        (vk::DescriptorPoolCreateFlags) 0,
                        numSetsToReserve,
                        descriptorSizes
                )
        );

    }

    std::vector<vk::DescriptorSet> DescriptorAllocator::allocate(size_t count) {
        std::vector<vk::DescriptorSet> sets(count);
        vk::DescriptorSet* basePtr = sets.data();
        size_t pending = count;
        do {
            un::DescriptorPool top = freePools.top();
            size_t available = top.getFreeSets();
            size_t toAllocate;
            if (available >= pending) {
                toAllocate = pending;
            } else {
                toAllocate = available;
            }
            vk::DescriptorSet* ptr = basePtr + (count - pending);
            top.allocate(toAllocate, ptr, owningDevice, vulkanLayout);
            if (top.isFull()) {
                create();
            }
            pending -= toAllocate;
        } while (pending > 0);

        return sets;
    }

    vk::DescriptorSet DescriptorAllocator::allocate() {
        un::DescriptorPool top = freePools.top();
        vk::DescriptorSet set;
        if (top.isFull()) {
            top = create();
        }
        top.allocate(1, &set, owningDevice, vulkanLayout);
        return set;
    }

    DescriptorAllocator::~DescriptorAllocator() {
        while (!freePools.empty()) {
            const vk::DescriptorPool& toFree = freePools.top().getPool();
            owningDevice.destroy(toFree);
            freePools.pop();
        }
    }

    un::DescriptorPool DescriptorAllocator::create() {
        return freePools.emplace(owningDevice, layout, poolReserveCount);
    }

    bool DescriptorPool::isFull() {
        return freeSets == 0;
    }

    const vk::DescriptorPool& DescriptorPool::getPool() const {
        return pool;
    }

    size_t DescriptorPool::getFreeSets() const {
        return freeSets;
    }

    size_t DescriptorPool::getNumSets() const {
        return numSets;
    }

    size_t DescriptorPool::getUsedSets() const {
        return numSets - freeSets;
    }

    void DescriptorPool::assertCanAllocate(size_t count) const {
        if (count > freeSets) {
            std::ostringstream str;
            str << "Pool is unable to allocate " << count << " descriptor sets, has " << freeSets;
            str << " free out of " << numSets;
            throw std::runtime_error(str.str());
        }
    }

    void DescriptorPool::allocate(
            size_t count,
            vk::DescriptorSet* setPtr,
            vk::Device device,
            vk::DescriptorSetLayout layout
    ) {
        assertCanAllocate(count);
        freeSets -= count;
        std::vector<vk::DescriptorSetLayout> layouts(count);
        for (size_t i = 0; i < count; ++i) {
            layouts[i] = layout;
        }
        vk::DescriptorSetAllocateInfo info(
                pool,
                count,
                layouts.data()
        );
        vkCall(device.allocateDescriptorSets(&info, setPtr));

    }

    DescriptorAllocator::DescriptorAllocator(
            DescriptorLayout&& oldLayout,
            vk::Device owningDevice,
            size_t numSetsPerPool,
            vk::ShaderStageFlagBits shaderStageFlags
    ) : layout(std::move(oldLayout)), poolReserveCount(numSetsPerPool), owningDevice(owningDevice) {
        create();
        vulkanLayout = layout.build(owningDevice, shaderStageFlags);
    }
}