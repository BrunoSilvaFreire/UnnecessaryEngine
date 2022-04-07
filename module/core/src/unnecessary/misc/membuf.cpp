#include "unnecessary/memory/membuf.h"

namespace un {

    Buffer::Buffer(const MemoryBuffer<u8>& other) : MemoryBuffer(other) { }

    Buffer::Buffer() { }

    Buffer::Buffer(size_t size, bool zero) : MemoryBuffer(size) {
        if (zero) {
            setZero();
        }

    }

    Buffer::Buffer(const std::vector<u8>& vector) {
        auto len = vector.size();
        resize(len);
        std::memcpy(ptr, vector.data(), len);
    }

    Buffer::Buffer(Buffer&& other) noexcept: un::MemoryBuffer<u8>(std::move(other)) {

    }


}