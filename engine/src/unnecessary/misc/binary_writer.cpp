#include <unnecessary/misc/binary_writer.h>

namespace un {


    u64 BinaryWriter::getFreeSize() {
        return size - ptr;
    }

    BinaryWriter::BinaryWriter(u64 bufSize) : buf(new u8[bufSize]), size(bufSize), ptr(0) {

    }

    BinaryWriter::~BinaryWriter() {
        delete[]buf;
    }

    void BinaryWriter::skip(size_t numBytes) {
        ptr += numBytes;
    }

    u8* BinaryWriter::getBuffer() const {
        return buf;
    }
}