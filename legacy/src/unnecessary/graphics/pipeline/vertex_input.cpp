#include <unnecessary/graphics/pipeline/vertex_input.h>

namespace un {

    const std::vector<vk::VertexInputBindingDescription>& VertexInputData::getInputBindings() const {
        return inputBindings;
    }

    const std::vector<vk::VertexInputAttributeDescription>& VertexInputData::getInputAttributes() const {
        return inputAttributes;
    }
}