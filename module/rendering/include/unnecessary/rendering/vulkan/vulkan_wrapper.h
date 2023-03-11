//
// Created by bruno on 22/01/2022.
//

#ifndef UNNECESSARYENGINE_VULKAN_WRAPPER_H
#define UNNECESSARYENGINE_VULKAN_WRAPPER_H

#include <unnecessary/def.h>

namespace un {

    template<typename TVulkanValue>
    class vulkan_wrapper {
    public:
        using value_type = TVulkanValue;
    protected:
        value_type _wrapped;

        vulkan_wrapper() : _wrapped(nullptr) {
        }

    public:
        vulkan_wrapper(value_type wrapped) : _wrapped(wrapped) {
        }

        operator value_type() {
            return _wrapped;
        }

        value_type operator*() {
            return _wrapped;
        }

        value_type* operator->() {
            return _wrapped;
        }

        const value_type& operator*() const {
            return _wrapped;
        }

        const value_type* operator->() const {
            return &_wrapped;
        }
    };
}
#endif //UNNECESSARYENGINE_VULKAN_WRAPPER_H
