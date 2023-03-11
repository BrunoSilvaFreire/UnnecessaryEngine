#include "unnecessary/memory/binary_writer.h"

namespace un {
    u64 binary_writer::get_free_size() const {
        return _buffer.size() - _ptr;
    }

    binary_writer::binary_writer(u64 bufSize) : _buffer(bufSize), _ptr(0) {
    }

    void binary_writer::skip(size_t numBytes) {
        _ptr += numBytes;
    }

    u8* binary_writer::get_buffer() const {
        return _buffer.operator->();
    }
}
