#ifndef UNNECESSARYENGINE_DESCRIPTOR_LAYOUT_H
#define UNNECESSARYENGINE_DESCRIPTOR_LAYOUT_H

#include <vulkan/vulkan.hpp>
#include <unnecessary/algorithm/layout.h>
#include <unnecessary/def.h>


namespace un {
    struct DescriptorReference {
        explicit DescriptorReference(u32 set = 0, u32 binding = 0);

        u32 set;
        u32 binding;

        bool operator==(const DescriptorReference &rhs) const;

        bool operator<(const DescriptorReference &rhs) const;

        bool operator>(const DescriptorReference &rhs) const;

        bool operator<=(const DescriptorReference &rhs) const;

        bool operator>=(const DescriptorReference &rhs) const;

        bool operator!=(const DescriptorReference &rhs) const;
    };

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


    class DescriptorSetLayout : public Layout<DescriptorElement> {
    public:
        static DescriptorSetLayout EMPTY_LAYOUT;

        static vk::DescriptorSetLayout build(
                std::vector<vk::DescriptorSetLayoutBinding> bindings,
                vk::Device owningDevice,
                vk::ShaderStageFlags shaderStageFlags
        );

        void withStandardCameraMatrices();

        void withGlobalSceneLighting();

        const un::DescriptorElement &getDescriptor(size_t index) const ;

        template<typename T>
        void push(const std::string &name, vk::DescriptorType type = vk::DescriptorType::eUniformBuffer) {
            elements.emplace_back(name, type, sizeof(T));
        }

        vk::DescriptorSetLayout build(
                vk::Device owningDevice,
                vk::ShaderStageFlags shaderStageFlags
        ) const;

        bool isEmpty() const;
    };
}

namespace std {
    template<>
    struct hash<un::DescriptorReference> {
        std::size_t operator()(const un::DescriptorReference &reference) const noexcept;
    };
}
#endif
