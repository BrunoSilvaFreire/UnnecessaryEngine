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

TEST(allocators, multiple_allocations) {
    std::size_t pageSize = 32;
    std::size_t numAllocs = pageSize * 32;
    un::PoolAllocator<MyStruct> allocator(pageSize);
    for (std::size_t i = 0; i < numAllocs; ++i) {
        MyStruct& aStruct = *allocator.construct(0, 0, i, i);
        ASSERT_EQ(aStruct.a, 0);
        ASSERT_EQ(aStruct.b, 0);
        ASSERT_EQ(aStruct.c, i);
        ASSERT_EQ(aStruct.d, i);
    }
}

TEST(allocators, find_correct_page) {
    std::size_t pageSize = 32;
    std::size_t numPagesToFill = (pageSize * 4);
    un::PoolAllocator<MyStruct> allocator(pageSize);
    auto first = allocator.construct(1, 2, 3, 4);
    for (std::size_t i = 0; i < numPagesToFill; ++i) {
        allocator.construct(i, i, i, i);
    }
    auto page = allocator.findPage(first);
    EXPECT_EQ(page, allocator.getRoot());
    allocator.dispose(first);
}