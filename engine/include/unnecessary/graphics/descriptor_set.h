//
// Created by bruno on 21/06/2021.
//

#ifndef UNNECESSARYENGINE_DESCRIPTOR_SET_H
#define UNNECESSARYENGINE_DESCRIPTOR_SET_H

#include <unnecessary/graphics/descriptor_set.h>

namespace un {
    class DescriptorSet {
    private:
        vk::DescriptorSet descriptorSet;
    public:
        explicit DescriptorSet(vk::Device device) {
            descriptorSet = device.allocateDescriptorSets(
                    vk::DescriptorSetAllocateInfo(

                    )
            );
        }
    };
}
#endif
