#include "unnecessary/memory/membuf.h"

namespace un {
    byte_buffer::byte_buffer(const buffer<u8>& other) : buffer(other) {
    }

    byte_buffer::byte_buffer() {
    }

    byte_buffer::byte_buffer(size_t size, bool zero) : buffer(size) {
        if (zero) {
            zero_buffer();
        }
    }

    byte_buffer::byte_buffer(const std::vector<u8>& vector) {
        auto len = vector.size();
        resize(len);
        std::memcpy(_ptr, vector.data(), len);
    }

    byte_buffer::byte_buffer(byte_buffer&& other) noexcept: buffer<u8>(std::move(other)) {
    }
}
