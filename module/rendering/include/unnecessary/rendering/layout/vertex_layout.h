//
// Created by brunorbsf on 17/12/2021.
//

#ifndef UNNECESSARYENGINE_VERTEX_LAYOUT_H
#define UNNECESSARYENGINE_VERTEX_LAYOUT_H

#include <unnecessary/def.h>
#include <unnecessary/rendering/layout/layout.h>
#include <vulkan/vulkan.hpp>

namespace un {
    struct vertex_attribute {
    private:
        std::string _name;
        /**
         * How many _all there are
         */
        u8 _count;
        /**
         * The size in bytes of a single element
         */
        u8 _singleSize;
        vk::Format _format;

    public:
        vertex_attribute(
            std::string name,
            u8 count,
            u8 singleSize,
            vk::Format format
        );

        /**
         * How many items does this input has? (vec2 = 2, mat4 = 16)
         */
        u8 get_count() const;

        u8 get_single_size() const;

        /**
         * @return The total size in bytes this element occupies
         */
        std::size_t get_size() const;

        vk::Format get_format() const;

        const std::string& get_name() const;
    };

    class vertex_layout : public layout<vertex_attribute> {
    public:
        std::size_t get_stride() const;
    };
}
#endif
