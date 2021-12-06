#include <unnecessary/graphics/pipeline/graphics_pipeline_layout.h>

namespace un {

    void GraphicsPipelineLayout::push(un::DescriptorSet&& layout) {
        elements.push_back(std::move(layout));
    }

    const un::DescriptorSet& GraphicsPipelineLayout::getSet(size_t index) const {
        return elements[index];
    }

    const std::vector<un::DescriptorSet>&
    GraphicsPipelineLayout::getDescriptorLayouts() const {
        return elements;
    }
}