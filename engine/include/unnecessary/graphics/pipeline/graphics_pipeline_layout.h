#ifndef UNNECESSARYENGINE_GRAPHICS_PIPELINE_LAYOUT_H
#define UNNECESSARYENGINE_GRAPHICS_PIPELINE_LAYOUT_H

#include <unnecessary/algorithm/layout.h>
#include <unnecessary/def.h>
#include <unnecessary/graphics/descriptors/descriptor_layout.h>
#include <unnecessary/graphics/descriptors/descriptor_set.h>

namespace un {
    class GraphicsPipelineLayout : Layout<un::DescriptorResource> {

    public:
        void push(un::DescriptorResource&& layout);

        const un::DescriptorResource& getSet(size_t index) const;

        const std::vector<un::DescriptorResource>& getDescriptorLayouts() const;
    };
}
#endif
