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
    class buffer {
    public:
        using element_type = T;
        using ptr = T*;

    private:
        void assert_within_bounds(size_t index) const {
            if (index >= _count) {
                throw std::runtime_error(
                    "Attempted to read memory outside of buffer range."
                );
            }
        }

    protected:
        std::size_t _count;
        ptr _ptr;

        void allocate(size_t numElements) {
            _ptr = static_cast<ptr>(malloc(sizeof(T) * numElements));
            _count = numElements;
        }

    public:
        /**
         * @return The size of this buffer in bytes
         */
        std::size_t size() const {
            return _count * sizeof(T);
        }

        /**
         * @return The number of _all in this buffer
         */
        std::size_t get_count() const {
            return _count;
        }

        const ptr offset(std::size_t offsetInBytes) const {
            return data() + offsetInBytes;
        }

        ptr offset(std::size_t offsetInBytes) {
            return data() + offsetInBytes;
        }

        u8* operator->() {
            return _ptr;
        }

        u8* operator->() const {
            return _ptr;
        }

        const u8* data() const {
            return operator->();
        }

        u8* data() {
            return operator->();
        }

        ~buffer() {
            if (is_allocated()) {
                free(_ptr);
                _ptr = nullptr;
            }
        }

        explicit buffer(std::size_t count) : _count(count), _ptr(nullptr) {
            allocate(count);
        }

        buffer(const buffer<T>& other) : _count(0), _ptr(nullptr) {
            allocate(other._count);
            std::memcpy(_ptr, other._ptr, size());
        }

        buffer(buffer<T>&& moved) noexcept:
            _count(std::move(moved._count)),
            _ptr(std::move(moved._ptr)) {
        }

        buffer() : _count(0), _ptr(nullptr) {
        }

        bool is_allocated() const {
            return _count > 0;
        }

        void resize(std::size_t newSize) {
            if (is_allocated()) {
                if (newSize >= 0) {
                    _ptr = static_cast<u8*>(realloc(_ptr, newSize));
                }
                else {
                    free(_ptr);
                    _ptr = nullptr;
                }
                _count = newSize;
            }
            else {
                allocate(newSize);
            }
        }

        void zero_buffer() const {
            std::memset(_ptr, 0, _count * sizeof(T));
        }

        T& operator[](std::size_t index) {
#ifdef DEBUG
            assert_within_bounds(index);
#endif
            return _ptr[index];
        }

        const T& operator[](std::size_t index) const {
#ifdef DEBUG
            assert_within_bounds(index);
#endif
            return _ptr[index];
        }

        const ptr begin() const {
            return _ptr;
        }

        const ptr end() const {
            return _ptr + _count;
        }

        ptr begin() {
            return _ptr;
        }

        ptr end() {
            return _ptr + _count;
        }

        friend std::ostream& operator<<(std::ostream& os, const buffer& buffer) {
            os.write(buffer._ptr, buffer.size());
            return os;
        }
    };

    class byte_buffer : public buffer<u8> {
    public:
        explicit byte_buffer(const std::vector<u8>& vector);

        explicit byte_buffer(const buffer<u8>& other);

        byte_buffer();

        byte_buffer(byte_buffer&& other) noexcept;

        template<typename TElement>
        static byte_buffer from_vector(const std::vector<TElement>& vector) {
            std::size_t bufSize = vector.size() * sizeof(TElement);
            byte_buffer buf(bufSize, false);
            buf.copy_from(vector.data());
            return buf;
        }

        explicit byte_buffer(size_t size, bool zero = true);

        template<typename T>
        void write(std::size_t index, const T& value) {
            _ptr[index] = value;
        }

        template<typename T>
        T read(std::size_t index) {
            return _ptr[index];
        }

        void copy_from(const void* data) {
            std::memcpy(_ptr, data, _count);
        }

        void copy_from(const void* data, std::size_t offset, std::size_t length) {
            std::memcpy(_ptr + offset, data, length);
        }

        byte_buffer& operator=(const std::string& string) {
            resize(string.size());
            copy_from(string.data(), 0, string.size());
            return *this;
        }
    };
}
#endif
