
#ifndef UNNECESSARYENGINE_VERTEX_INPUT_H
#define UNNECESSARYENGINE_VERTEX_INPUT_H

#include <vector>
#include <unnecessary/def.h>
#include <vulkan/vulkan.hpp>

namespace un {
    class VertexInputData {
    private:
        std::vector<vk::VertexInputBindingDescription> inputBindings;
        std::vector<vk::VertexInputAttributeDescription> inputAttributes;
    public:
        const std::vector<vk::VertexInputBindingDescription>& getInputBindings() const;

        const std::vector<vk::VertexInputAttributeDescription>&
        getInputAttributes() const;

        void addVertexAttribute(u32 location, u32 binding, vk::Format format) {
            vk::VertexInputAttributeDescription attribute(

            );

        }
    };
}
#endif