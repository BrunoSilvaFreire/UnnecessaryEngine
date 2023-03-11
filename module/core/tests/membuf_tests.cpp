//
// Created by bruno on 06/12/2021.
//

#include <gtest/gtest.h>
#include "unnecessary/memory/membuf.h"
#include "unnecessary/memory/binary_writer.h"

#if DEBUG

TEST(memory, binary_writer_overflow) {
    const size_t length = 64;
    un::binary_writer writer(length);
    u8 values[length];
    writer.write(values, length);
    ASSERT_THROW(writer.write(0), std::runtime_error);
}

TEST(memory, buffer_overflow) {
    const size_t length = 64;
    un::byte_buffer writer(length);
    ASSERT_THROW(
        writer[length + 1],
        std::runtime_error
    );
}

#endif
