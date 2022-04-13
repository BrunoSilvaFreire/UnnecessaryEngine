//
// Created by bruno on 07/04/2022.
//

#ifndef UNNECESSARYENGINE_BUFFER_STREAM_H
#define UNNECESSARYENGINE_BUFFER_STREAM_H

#include <memory>
#include <unnecessary/memory/membuf.h>
#include <istream>

namespace un {
    template<typename TBuffer = un::Buffer>
    class BufferStream final : public std::streambuf {
    private:
        std::size_t index;
        std::shared_ptr<TBuffer> buffer;
        using TElement = typename TBuffer::ElementType;
    public:
        BufferStream(const std::shared_ptr<TBuffer>& buffer) : std::streambuf(), buffer(buffer) {
            TBuffer& buf = *buffer;
            auto start = reinterpret_cast<char*>(buf.begin());
            auto end = reinterpret_cast<char*>(buf.end());
            setg(start, start, end);
        }
    };

}
#endif
