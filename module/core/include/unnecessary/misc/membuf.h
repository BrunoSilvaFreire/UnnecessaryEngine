#ifndef UNNECESSARYENGINE_MEMBUF_H
#define UNNECESSARYENGINE_MEMBUF_H

#include <unnecessary/def.h>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

namespace un {
    template<typename T>
    class MemoryBuffer {
    public:
        typedef T ElementType;
    private:
        void assertWithinBounds(size_t index) const {
            if (index >= count) {
                throw std::runtime_error(
                    "Attempted to read memory outside of buffer range."
                );
            }
        }

    protected:
        std::size_t count;
        T* ptr;

        void allocate(size_t numElements) {
            ptr = static_cast<T*>(malloc(sizeof(T) * numElements));
            count = numElements;
        }

    public:
        /**
         * @return The size of this buffer in bytes
         */
        std::size_t size() const {
            return count * sizeof(T);
        }

        /**
         * @return The number of allElements in this buffer
         */
        std::size_t getCount() const {
            return count;
        }

        u8* operator->() {
            return ptr;
        }

        u8* const operator->() const {
            return ptr;
        }

        const u8* const data() const {
            return operator->();
        }

        u8* data() {
            return operator->();
        }

        ~MemoryBuffer() {
            if (isAllocated()) {
                free(ptr);
                ptr = nullptr;
            }
        }

        MemoryBuffer(std::size_t count) {
            allocate(count);
        }

        MemoryBuffer(const un::MemoryBuffer<T>& other) {
            allocate(other.count);
            std::memcpy(ptr, other.ptr, size());
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

        void setZero() const {
            std::memset(ptr, 0, count * sizeof(T));
        }

        T& operator[](std::size_t index) {
#ifdef DEBUG
            assertWithinBounds(index);
#endif
            return ptr[index];
        }


        const T& operator[](std::size_t index) const {
#ifdef DEBUG
            assertWithinBounds(index);
#endif
            return ptr[index];
        }
    };

    class Buffer : public MemoryBuffer<u8> {
    public:
        Buffer(const MemoryBuffer<u8>& other);

        Buffer();

        Buffer(size_t size, bool zero = true);

        template<typename T>
        void write(std::size_t index, const T& value) {
            ptr[index] = value;
        }

        template<typename T>
        T read(std::size_t index) {
            return ptr[index];
        }

    };

}
#endif
