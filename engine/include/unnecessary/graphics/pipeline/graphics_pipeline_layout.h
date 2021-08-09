#ifndef UNNECESSARYENGINE_GRAPHICS_PIPELINE_LAYOUT_H
#define UNNECESSARYENGINE_GRAPHICS_PIPELINE_LAYOUT_H

#include <unnecessary/algorithm/layout.h>
#include <unnecessary/graphics/descriptors/descriptor_layout.h>
#include <unnecessary/def.h>

namespace un {
    class GraphicsPipelineLayout : Layout<un::DescriptorSetLayout> {
    public:
        void push(un::DescriptorSetLayout &&layout);

        const un::DescriptorSetLayout &getSet(size_t index) const;

        const std::vector<un::DescriptorSetLayout> &getDescriptorLayouts() const;
    };
}
#endif
