#include <unnecessary/misc/binary_writer.h>

namespace un {


    u64 BinaryWriter::getFreeSize() const {
        return buf.size() - ptr;
    }

    BinaryWriter::BinaryWriter(u64 bufSize) : buf(bufSize), ptr(0) {

    }

    void BinaryWriter::skip(size_t numBytes) {
        ptr += numBytes;
    }

    u8* BinaryWriter::getBuffer() const {
        return buf.operator->();
    }
}