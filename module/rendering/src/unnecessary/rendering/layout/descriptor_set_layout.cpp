#include <unnecessary/rendering/layout/descriptor_set_layout.h>

namespace un {
    const std::string& descriptor::get_name() const {
        return _name;
    }

    vk::DescriptorType descriptor::get_type() const {
        return type;
    }

    descriptor::descriptor(
        const std::string& name,
        vk::DescriptorType type
    ) : _name(name),
        type(type) {
    }

    vk::DescriptorSetLayout descriptor_set_layout::build(vk::Device device) {
        std::vector<vk::DescriptorSetLayoutBinding> items;
        for (std::size_t i = 0; i < _elements.size(); ++i) {
            items.emplace_back(
                vk::DescriptorSetLayoutBinding(
                    i,
                    _elements[i].get_type(),
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

    descriptor_set_layout::descriptor_set_layout(
        const vk::ShaderStageFlags& stageFlags
    ) : stageFlags(stageFlags) {
    }
}
