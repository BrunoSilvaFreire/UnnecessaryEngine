//
// Created by bruno on 07/04/2022.
//

#ifndef UNNECESSARYENGINE_BUFFER_STREAM_H
#define UNNECESSARYENGINE_BUFFER_STREAM_H

#include <memory>
#include <unnecessary/memory/membuf.h>
#include <istream>

namespace un {
    template<typename t_buffer = byte_buffer>
    class buffer_stream final : public std::streambuf {
    private:
        std::size_t _index;
        std::shared_ptr<t_buffer> _buffer;
        using t_element = typename t_buffer::element_type;

    public:
        buffer_stream(const std::shared_ptr<t_buffer>& buffer) : std::streambuf(), _buffer(buffer) {
            t_buffer& buf = *buffer;
            auto start = reinterpret_cast<char*>(buf.begin());
            auto end = reinterpret_cast<char*>(buf.end());
            setg(start, start, end);
        }
    };
}
#endif
