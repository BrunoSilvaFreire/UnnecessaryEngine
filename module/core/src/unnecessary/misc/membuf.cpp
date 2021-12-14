#include <unnecessary/misc/membuf.h>

namespace un {

    Buffer::Buffer(const MemoryBuffer<u8>& other) : MemoryBuffer(other) {}

    Buffer::Buffer() {}

    Buffer::Buffer(size_t size) : MemoryBuffer(size) {}

}