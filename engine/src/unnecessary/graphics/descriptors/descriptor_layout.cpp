#include <unnecessary/graphics/descriptors/descriptor_layout.h>
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

    const un::DescriptorElement& DescriptorSetLayout::getDescriptor(size_t index) const {
        return elements[index];
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

    bool DescriptorReference::operator<(const DescriptorReference& rhs) const {
        if (set < rhs.set)
            return true;
        if (rhs.set < set)
            return false;
        return binding < rhs.binding;
    }

    bool DescriptorReference::operator>(const DescriptorReference& rhs) const {
        return rhs < *this;
    }

    bool DescriptorReference::operator<=(const DescriptorReference& rhs) const {
        return !(rhs < *this);
    }

    bool DescriptorReference::operator>=(const DescriptorReference& rhs) const {
        return !(*this < rhs);
    }

    DescriptorReference::DescriptorReference(u32 set, u32 binding) : set(set),
                                                                     binding(binding) {}

    bool DescriptorReference::operator==(const DescriptorReference& rhs) const {
        return set == rhs.set &&
               binding == rhs.binding;
    }

    bool DescriptorReference::operator!=(const DescriptorReference& rhs) const {
        return !(rhs == *this);
    }
}
namespace std {
    std::size_t
    hash<un::DescriptorReference>::operator()(const un::DescriptorReference& reference) const noexcept {
        return reference.set ^ (reference.binding << 1);
    }
}