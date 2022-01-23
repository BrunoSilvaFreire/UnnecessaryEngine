//
// Created by bruno on 22/01/2022.
//

#ifndef UNNECESSARYENGINE_VULKAN_WRAPPER_H
#define UNNECESSARYENGINE_VULKAN_WRAPPER_H

#include <unnecessary/def.h>

namespace un {

    template<typename TVulkanValue>
    class VulkanWrapper {
    public:
        typedef TVulkanValue ValueType;
    protected:
        ValueType _wrapped;

        VulkanWrapper() : _wrapped(nullptr) {
        }

    public:
        VulkanWrapper(ValueType wrapped) : _wrapped(wrapped) {
        }

        operator ValueType() {
            return _wrapped;
        }

        ValueType operator*() {
            return _wrapped;
        }

        ValueType* operator->() {
            return _wrapped;
        }

        const ValueType& operator*() const {
            return _wrapped;
        }

        const ValueType* operator->() const {
            return &_wrapped;
        }
    };
}
#endif //UNNECESSARYENGINE_VULKAN_WRAPPER_H
