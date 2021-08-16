#include <unnecessary/graphics/pipeline/graphics_pipeline_layout.h>

namespace un {

    void GraphicsPipelineLayout::push(un::DescriptorResource&& layout) {
        elements.push_back(std::move(layout));
    }

    const un::DescriptorResource& GraphicsPipelineLayout::getSet(size_t index) const {
        return elements[index];
    }

    const std::vector<un::DescriptorResource>&
    GraphicsPipelineLayout::getDescriptorLayouts() const {
        return elements;
    }
}