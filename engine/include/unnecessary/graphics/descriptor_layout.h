#ifndef UNNECESSARYENGINE_DESCRIPTOR_LAYOUT_H
#define UNNECESSARYENGINE_DESCRIPTOR_LAYOUT_H

#include <vulkan/vulkan.hpp>
#include <unnecessary/algorithm/layout.h>

namespace un {
    struct DescriptorElement {
    private:
        std::string name;
        vk::DescriptorType type;
        std::size_t size;
    public:
        DescriptorElement(std::string name, vk::DescriptorType type, size_t size);

        const std::string &getName() const;

        vk::DescriptorType getType() const;

        size_t getSize() const;
    };

    class DescriptorLayout : public Layout<DescriptorElement> {
    public:
        void withStandardCameraMatrices();

        template<typename T>
        void push(const std::string &name, vk::DescriptorType type = vk::DescriptorType::eUniformBufferDynamic) {
            elements.emplace_back(name, type, sizeof(T));
        }
    };
}
#endif
