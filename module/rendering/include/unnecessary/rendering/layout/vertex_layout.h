//
// Created by brunorbsf on 17/12/2021.
//

#ifndef UNNECESSARYENGINE_VERTEX_LAYOUT_H
#define UNNECESSARYENGINE_VERTEX_LAYOUT_H

#include <unnecessary/def.h>
#include <unnecessary/rendering/layout/layout.h>
#include <vulkan/vulkan.hpp>

namespace un {
    struct VertexAttribute {
    private:
        std::string name;
        /**
         * How many _all there are
         */
        u8 count;
        /**
         * The size in bytes of a single element
         */
        u8 singleSize;
        vk::Format format;
    public:
        VertexAttribute(
            std::string name,
            u8 count,
            u8 singleSize,
            vk::Format format
        );

        /**
         * How many items does this input has? (vec2 = 2, mat4 = 16)
         */
        u8 getCount() const;

        u8 getSingleSize() const;

        /**
         * @return The total size in bytes this element occupies
         */
        std::size_t getSize() const;

        vk::Format getFormat() const;

        const std::string& getName() const;
    };

    class VertexLayout : public un::Layout<un::VertexAttribute> {
    public:
        std::size_t getStride() const;
    };
}
#endif
