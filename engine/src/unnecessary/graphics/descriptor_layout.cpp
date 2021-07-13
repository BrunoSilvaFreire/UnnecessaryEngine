#include <unnecessary/graphics/descriptor_layout.h>
#include <unnecessary/graphics/matrices.h>

#include <utility>
#include <unnecessary/graphics/lighting.h>

namespace un {
    DescriptorSetLayout DescriptorSetLayout::EMPTY_LAYOUT = DescriptorSetLayout();

    void DescriptorSetLayout::withStandardCameraMatrices() {
        push<un::Matrices>("matrices");
    }

    vk::DescriptorSetLayout DescriptorSetLayout::build(
            vk::Device owningDevice,
            vk::ShaderStageFlags shaderStageFlags
    ) const {
        std::vector<vk::DescriptorSetLayoutBinding> bindings(elements.size());
        for (u32 i = 0; i < elements.size(); ++i) {
            const DescriptorElement& element = elements[i];
            bindings[i] = vk::DescriptorSetLayoutBinding(
                    i,
                    element.getType(),
                    1,
                    shaderStageFlags
            );
        }
        return owningDevice.createDescriptorSetLayout(
                vk::DescriptorSetLayoutCreateInfo(
                        (vk::DescriptorSetLayoutCreateFlags) 0,
                        bindings
                )
        );
    }

    bool DescriptorSetLayout::isEmpty() const {
        return elements.empty();
    }

    void DescriptorSetLayout::withGlobalSceneLighting() {
        push<un::SceneLightingData>("sceneLighting");
    }

    DescriptorElement::DescriptorElement(
            std::string name,
            vk::DescriptorType type,
            size_t size
    ) : name(std::move(name)),
        type(type),
        size(size) {}

    const std::string& DescriptorElement::getName() const {
        return name;
    }

    vk::DescriptorType DescriptorElement::getType() const {
        return type;
    }

    size_t DescriptorElement::getSize() const {
        return size;
    }
}