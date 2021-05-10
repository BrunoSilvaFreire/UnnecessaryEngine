#ifndef UNNECESSARYENGINE_VERTEX_LAYOUT_H
#define UNNECESSARYENGINE_VERTEX_LAYOUT_H

#include <vulkan/vulkan.hpp>
#include <unnecessary/def.h>
#include <unnecessary/algorithm/layout.h>

namespace un {
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
    public:
        VertexInput(u8 count, u8 size, vk::Format format);

        u8 getCount() const;

        u8 getElementSize() const;

        vk::Format getFormat() const;
    };


    class VertexLayout : public un::Layout<::un::VertexInput> {

    public:
        template<typename T>
        void push(u8 count, vk::Format format);

        template<>
        void push<f32>(u8 count, vk::Format format);

        template<>
        void push<u8>(u8 count, vk::Format format);

        u32 getStride();

    };

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
