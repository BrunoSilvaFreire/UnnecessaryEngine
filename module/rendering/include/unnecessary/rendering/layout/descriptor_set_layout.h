//
// Created by bruno on 20/12/2021.
//

#ifndef UNNECESSARYENGINE_DESCRIPTOR_SET_LAYOUT_H
#define UNNECESSARYENGINE_DESCRIPTOR_SET_LAYOUT_H

#include <string>
#include <vulkan/vulkan.hpp>
#include <unnecessary/rendering/layout/layout.h>

namespace un {
    class Descriptor {
    private:
        std::string name;
        vk::DescriptorType type;
    public:
        Descriptor(
            const std::string& name,
            vk::DescriptorType type
        );

        const std::string& getName() const;

        vk::DescriptorType getType() const;
    };

    class DescriptorSetLayout : public un::Layout<un::Descriptor> {
    private:
        vk::ShaderStageFlags stageFlags;
    public:
        DescriptorSetLayout(
            const vk::ShaderStageFlags& stageFlags
        );

        vk::DescriptorSetLayout build(vk::Device device);
    };
}
#endif //UNNECESSARYENGINE_DESCRIPTOR_SET_LAYOUT_H
