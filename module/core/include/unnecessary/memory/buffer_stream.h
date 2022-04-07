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
        std::shared_ptr<TBuffer> buffer = 0;
    public:
        BufferStream(const std::shared_ptr<TBuffer>& buffer) : std::streambuf(), buffer(buffer) { }

        template<typename TElement>
        un::BufferStream<TBuffer>& read(TElement* s, std::streamsize n) {
            std::memcpy(s, buffer->offset(index), n);
            index += n;
            return *this;
        }

        bool good() const {
            return index < buffer->size();
        }
    };

}
#endif
