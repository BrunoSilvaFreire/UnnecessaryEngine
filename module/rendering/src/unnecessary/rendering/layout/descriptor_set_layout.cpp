#include <unnecessary/rendering/layout/descriptor_set_layout.h>

namespace un {
    const std::string& un::Descriptor::getName() const {
        return name;
    }

    vk::DescriptorType un::Descriptor::getType() const {
        return type;
    }

    Descriptor::Descriptor(
        const std::string& name,
        vk::DescriptorType type
    ) : name(name),
        type(type) {}

    vk::DescriptorSetLayout DescriptorSetLayout::build(vk::Device device) {
        std::vector<vk::DescriptorSetLayoutBinding> items;
        for (std::size_t i = 0; i < elements.size(); ++i) {

            items.emplace_back(
                vk::DescriptorSetLayoutBinding(
                    i,
                    elements[i].getType(),
                    1,
                    stageFlags,
                    nullptr
                )
            );
        }
        return device.createDescriptorSetLayout(
            vk::DescriptorSetLayoutCreateInfo(
                static_cast<vk::DescriptorSetLayoutCreateFlags>(0),
                items
            )
        );
    }

    DescriptorSetLayout::DescriptorSetLayout(
        const vk::ShaderStageFlags& stageFlags
    ) : stageFlags(stageFlags) {

    }
}