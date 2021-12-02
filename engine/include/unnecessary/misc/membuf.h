#ifndef UNNECESSARYENGINE_MEMBUF_H
#define UNNECESSARYENGINE_MEMBUF_H

#include <unnecessary/def.h>
#include <cstdlib>

namespace un {
    template<typename T>
    class MemoryBuffer {
    protected:
        std::size_t count;
        T* ptr;

        void allocate(size_t numElements) {
            ptr = static_cast<T*>(malloc(sizeof(T) * numElements));
            count = numElements;
        }

    public:
        u8* operator->() {
            return ptr;
        }

        u8* const operator->() const {
            return ptr;
        }

        ~MemoryBuffer() {
            if (isAllocated()) {
                free(ptr);
            }
        }

        MemoryBuffer(std::size_t size) {
            allocate(size);
        }

        MemoryBuffer(const un::MemoryBuffer<T>& other) {
            allocate(other.size);
        }


        MemoryBuffer() : ptr(nullptr) {
        }

        bool isAllocated() const {
            return count > 0;
        }

        void resize(std::size_t newSize) {
            if (newSize == 0) {
                throw std::runtime_error("Cannot resize buffer to length 0.");
            }
            if (isAllocated()) {
                if (newSize >= 0) {
                    ptr = static_cast<u8*>(realloc(ptr, newSize));
                } else {
                    free(ptr);
                    ptr = nullptr;
                }
                count = newSize;
            } else {
                allocate(newSize);
            }
        }

        T& operator[](std::size_t index) {
            return ptr[index];
        }

        const T& operator[](std::size_t index) const {
            return ptr[index];
        }
    };

    class Buffer : public MemoryBuffer<u8> {

        template<typename T>
        void write(std::size_t index, const T& value) {
            *reinterpret_cast<T*>(ptr + index) = value;
        }

        template<typename T>
        T read(std::size_t index) {
            return *reinterpret_cast<T*>(ptr + index);
        }
    };

}
#endif
