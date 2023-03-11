#ifndef UNNECESSARYENGINE_BINARY_WRITER_H
#define UNNECESSARYENGINE_BINARY_WRITER_H

#include "unnecessary/def.h"
#include "membuf.h"

namespace un {
    class binary_writer {
    private:
        byte_buffer _buffer;
        u64 _ptr;

    public:
        explicit binary_writer(u64 bufSize);

        u64 get_free_size() const;

        u8* get_buffer() const;

        template<typename T>
        void write(T* value, u64 count = 1) {
            const size_t elementSize = sizeof(T);
#if DEBUG
            const size_t requiredSpace = elementSize * count;
            if (requiredSpace > get_free_size()) {
                throw std::runtime_error("Binary writer buffer overflow.");
            }
#endif
            for (u64 j = 0; j < count; ++j) {
                _buffer.write<T>(_ptr, value[j]);
                _ptr += elementSize;
            }
        }

        template<typename T>
        void write(const T& value) {
            write(&value);
        }

        void skip(size_t numBytes);
    };
}
#endif
