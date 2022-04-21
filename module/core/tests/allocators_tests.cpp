#include <gtest/gtest.h>
#include <unnecessary/algorithms/pool_allocator.h>

struct MyStruct {
    int a;
    int b;
    short c;
    short d;
    union {
        int a;
        struct {
            short as;
            short bs;
        };
    } customUnion;

    MyStruct(int a, int b, short c, short d) : a(a), b(b), c(c), d(d) {
        customUnion.a = std::numeric_limits<int>::max();
    }
};

TEST(allocators, pool_allocator) {
    un::PoolAllocator<MyStruct> allocator(32);
    const std::size_t numAllocs = 1 << 12;
    for (std::size_t i = 0; i < numAllocs; ++i) {
        MyStruct& aStruct = allocator.construct(0, 0, i, i);
        ASSERT_EQ(aStruct.a, 0);
        ASSERT_EQ(aStruct.b, 0);
        ASSERT_EQ(aStruct.c, i);
        ASSERT_EQ(aStruct.d, i);
    }
}