#include <unnecessary/graphics/descriptor_layout.h>
#include <unnecessary/graphics/matrices.h>

#include <utility>

namespace un {

    void DescriptorLayout::withStandardCameraMatrices() {
        push<un::Matrices>("matrices");
    }

    DescriptorElement::DescriptorElement(
            std::string name,
            vk::DescriptorType type,
            size_t size
    ) : name(std::move(name)),
        type(type),
        size(size) {}

    const std::string &DescriptorElement::getName() const {
        return name;
    }

    vk::DescriptorType DescriptorElement::getType() const {
        return type;
    }

    size_t DescriptorElement::getSize() const {
        return size;
    }
}