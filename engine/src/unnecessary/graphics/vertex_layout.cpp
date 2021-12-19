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
    void VertexLayout::push<f32>(
        u8 count,
        vk::Format format,
        un::CommonVertexAttribute type
    ) {
        elements.emplace_back(count, sizeof(f32), format, type);
    }

    template<>
    void VertexLayout::push<u8>(
        u8 count, vk::Format format,
        un::CommonVertexAttribute type
    ) {
        elements.emplace_back(count, sizeof(u8), format, type);
    }

    VertexInput::VertexInput(
        u8 count,
        u8 size,
        vk::Format format,
        un::CommonVertexAttribute type
    ) : count(count), size(size), format(format), type(type) {}

    std::size_t VertexInput::getLength() const {
        return count * size;
    }

    CommonVertexAttribute VertexInput::getType() const {
        return type;
    }

    u32 VertexLayout::getStride() const {
        u32 stride = 0;
        for (const un::VertexInput& input: elements) {
            stride += input.getLength();
        }
        return stride;
    }

    BoundVertexLayout::BoundVertexLayout(u32 binding) : binding(binding) {}

    u32 BoundVertexLayout::getBinding() const {
        return binding;
    }

    BoundVertexLayout::BoundVertexLayout(VertexLayout layout, u32 binding) : binding(
        binding
    ) {
        for (un::VertexInput& input: layout.getElements()) {
            elements.push_back(input);
        }
    }


}