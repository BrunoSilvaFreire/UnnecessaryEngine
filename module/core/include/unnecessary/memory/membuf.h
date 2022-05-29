#ifndef UNNECESSARYENGINE_MEMBUF_H
#define UNNECESSARYENGINE_MEMBUF_H

#include "unnecessary/def.h"
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <vector>
#include <ostream>

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

        const T* offset(std::size_t offsetInBytes) const {
            return data() + offsetInBytes;
        }

        T* offset(std::size_t offsetInBytes) {
            return data() + offsetInBytes;
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

        explicit MemoryBuffer(std::size_t count) : count(count), ptr(nullptr) {
            allocate(count);
        }

        MemoryBuffer(const un::MemoryBuffer<T>& other) : ptr(nullptr), count(0) {
            allocate(other.count);
            std::memcpy(ptr, other.ptr, size());
        }

        MemoryBuffer(
            un::MemoryBuffer<T>&& moved
        ) noexcept: ptr(std::move(moved.ptr)), count(std::move(moved.count)) { }

        MemoryBuffer() : ptr(nullptr), count(0) { }

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

        void zeroBuffer() const {
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

        const T* const begin() const {
            return ptr;
        }

        const T* const end() const {
            return ptr + count;
        }

        T* const begin() {
            return ptr;
        }

        T* const end() {
            return ptr + count;
        }

        friend std::ostream& operator<<(std::ostream& os, const MemoryBuffer& buffer) {
            os.write(buffer.ptr, buffer.size());
            return os;
        }
    };

    class Buffer : public MemoryBuffer<u8> {
    public:
        explicit Buffer(const std::vector<u8>& vector);

        explicit Buffer(const MemoryBuffer<u8>& other);

        Buffer();

        Buffer(un::Buffer&& other) noexcept;

        template<typename TElement>
        static un::Buffer fromVector(const std::vector<TElement>& vector) {
            std::size_t bufSize = vector.size() * sizeof(TElement);
            un::Buffer buf(bufSize, false);
            buf.copyFrom(vector.data());
            return buf;
        }

        explicit Buffer(size_t size, bool zero = true);

        template<typename T>
        void write(std::size_t index, const T& value) {
            ptr[index] = value;
        }

        template<typename T>
        T read(std::size_t index) {
            return ptr[index];
        }

        void copyFrom(const void* data) {
            std::memcpy(ptr, data, count);
        }

        void copyFrom(const void* data, std::size_t offset, std::size_t length) {
            std::memcpy(ptr + offset, data, length);
        }

        un::Buffer& operator=(const std::string& string) {
            resize(string.size());
            copyFrom(reinterpret_cast<const void*>(string.data()), 0, string.size());
            return *this;
        }
    };

}
#endif
