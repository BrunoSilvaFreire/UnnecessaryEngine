//
// Created by bruno on 20/12/2021.
//

#ifndef UNNECESSARYENGINE_DESCRIPTOR_SET_LAYOUT_H
#define UNNECESSARYENGINE_DESCRIPTOR_SET_LAYOUT_H

#include <string>
#include <vulkan/vulkan.hpp>
#include <unnecessary/rendering/layout/layout.h>

namespace un {
    class descriptor {
    private:
        std::string _name;
        vk::DescriptorType _type;

    public:
        descriptor(
            const std::string& name,
            vk::DescriptorType type
        );

        const std::string& get_name() const;

        vk::DescriptorType get_type() const;
    };

    class descriptor_set_layout : public layout<descriptor> {
    private:
        vk::ShaderStageFlags _stageFlags;

    public:
        descriptor_set_layout(
            const vk::ShaderStageFlags& stageFlags
        );

        vk::DescriptorSetLayout build(vk::Device device);
    };
}
#endif //UNNECESSARYENGINE_DESCRIPTOR_SET_LAYOUT_H
