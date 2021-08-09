#include <unnecessary/graphics/pipeline/graphics_pipeline_layout.h>

namespace un {

    void GraphicsPipelineLayout::push(DescriptorSetLayout &&layout) {
        elements.push_back(std::move(layout));
    }

    const un::DescriptorSetLayout &GraphicsPipelineLayout::getSet(size_t index) const {
        return elements[index];
    }

    const std::vector<un::DescriptorSetLayout> &GraphicsPipelineLayout::getDescriptorLayouts() const {
        return elements;
    }
}