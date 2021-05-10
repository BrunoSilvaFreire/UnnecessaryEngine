#include <unnecessary/graphics/vertex_layout.h>

namespace un {

    u8 VertexInput::getCount() const {
        return count;
    }

    u8 VertexInput::getElementSize() const {
        return size;
    }

    vk::Format VertexInput::getFormat() const {
        return format;
    }

    template<>
    void VertexLayout::push<f32>(u8 count, vk::Format format) {
        elements.emplace_back(count, sizeof(f32), format);
    }

    template<>
    void VertexLayout::push<u8>(u8 count, vk::Format format) {
        elements.emplace_back(count, sizeof(u8), format);
    }

    VertexInput::VertexInput(
            u8 count,
            u8 size,
            vk::Format format
    ) : count(count), size(size), format(format) {}

    u32 VertexLayout::getStride() {
        u32 stride = 0;
        for (VertexInput &input : elements) {
            stride += input.getCount() * input.getElementSize();
        }
        return stride;
    }

    BoundVertexLayout::BoundVertexLayout(u32 binding) : binding(binding) {}

    u32 BoundVertexLayout::getBinding() const {
        return binding;
    }

    BoundVertexLayout::BoundVertexLayout(VertexLayout layout, u32 binding) : binding(binding) {
        for (un::VertexInput &input : layout.getElements()) {
            elements.push_back(input);
        }
    }

}