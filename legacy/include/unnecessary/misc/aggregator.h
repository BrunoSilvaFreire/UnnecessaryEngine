#ifndef UNNECESSARYENGINE_AGGREGATOR_H
#define UNNECESSARYENGINE_AGGREGATOR_H

#include <vector>
#include <vulkan/vulkan.hpp>

namespace un {
    /**
     * Helper class that makes it easier to allocate (usually vulkan) stuff inside a loop.
     */
    template<typename T>
    class Aggregator {
    private:
        std::size_t head;
        std::vector<T> content;
    public:
        void push(T&& data) {
            content.emplace_back(std::move(data));
        }

        template<typename ...Args>
        void emplace(Args... args) {
            content.emplace_back(std::forward<Args...>(args...));
        }

        void begin() {
            head = content.size();
        }

        T* data() {
            return content.data() + head;
        }

        const T* data() const {
            return content.data() + head;
        }

        std::size_t size() const {
            return content.size() - head;
        }

        operator vk::ArrayProxyNoTemporaries<const T>() const {
            return vk::ArrayProxyNoTemporaries<const T>(size(), data());
        }

        void reserve(std::size_t bytes) {
            content.reserve(bytes);
        }
    };
}
#endif
