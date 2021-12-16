#ifndef UNNECESSARYENGINE_VERTEX_LAYOUT_H
#define UNNECESSARYENGINE_VERTEX_LAYOUT_H

#include <vulkan/vulkan.hpp>
#include <unnecessary/def.h>
#include <unnecessary/algorithm/layout.h>

namespace un {
    enum class CommonVertexAttribute {
        eGeneric,
        ePosition,
        eNormal,
        eColor,
        eTexture
    };

    struct VertexInput {
    private:
        /**
         * How many elements there are
         */
        u8 count;
        /**
         * The size in bytes of a single element
         */
        u8 size;
        vk::Format format;
        un::CommonVertexAttribute type;
    public:
        VertexInput(
            u8 count,
            u8 size,
            vk::Format format,
            un::CommonVertexAttribute type = un::CommonVertexAttribute::eGeneric
        );

        u8 getCount() const;

        u8 getElementSize() const;

        std::size_t getLength() const;

        vk::Format getFormat() const;

        CommonVertexAttribute getType() const;
    };


    class VertexLayout : public un::Layout<un::VertexInput> {
    public:
        template<typename T>
        void push(
            u8 count,
            vk::Format format,
            un::CommonVertexAttribute type = un::CommonVertexAttribute::eGeneric
        );

        u32 getStride() const;

    };

    template<>
    void VertexLayout::push<f32>(
        u8 count,
        vk::Format format,
        un::CommonVertexAttribute type
    );

    template<>
    void VertexLayout::push<u8>(
        u8 count,
        vk::Format format,
        un::CommonVertexAttribute type
    );

    class BoundVertexLayout : public VertexLayout {
    private:
        u32 binding;
    public:
        explicit BoundVertexLayout(u32 binding = 0);

        explicit BoundVertexLayout(VertexLayout other, u32 binding = 0);


        u32 getBinding() const;
    };

}
#endif //UNNECESSARYENGINE_VERTEX_LAYOUT_H
