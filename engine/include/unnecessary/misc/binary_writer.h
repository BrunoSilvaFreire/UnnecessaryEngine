#ifndef UNNECESSARYENGINE_BINARY_WRITER_H
#define UNNECESSARYENGINE_BINARY_WRITER_H

#include <unnecessary/def.h>

namespace un {
    class BinaryWriter {
    private:
        u8* buf;
        u64 size;
        u64 ptr;
    public:
        explicit BinaryWriter(u64 bufSize);

        virtual ~BinaryWriter();

        u64 getFreeSize();

        u8* getBuffer() const;

        template<typename T>
        void write(T* value, u64 count = 1) {
            const size_t elementSize = sizeof(T);
#if DEBUG
            const size_t requiredSpace = elementSize * count;
            if (requiredSpace > getFreeSize()) {
                throw std::runtime_error("Binary writer buffer overflow.");
            }
#endif
            for (u64 j = 0; j < count; ++j) {
                std::memcpy(buf + ptr, value + j, elementSize);
                ptr += elementSize;
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