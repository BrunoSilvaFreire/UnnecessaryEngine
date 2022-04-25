#include <gtest/gtest.h>
#include <unnecessary/algorithms/n_tree.h>

TEST(trees, assignment_and_access) {
    un::QuadTree<int> tree;
    constexpr std::size_t Index = 0;
    un::QuadTree<int>::TPointer& ptr = tree.getChild<Index>();
    ASSERT_EQ(ptr, nullptr);
    ptr = tree.setChild<Index>(5);
    ASSERT_NE(ptr, nullptr);
    ASSERT_EQ(ptr->getData(), 5);
}

template<typename TTree>
void test_add_child() {
    TTree tree;
    constexpr std::size_t Count = TTree::num_children();
    for (int i = 0; i < Count; ++i) {
        ASSERT_NO_THROW(tree.addChild(i));
    }
    ASSERT_THROW(tree.addChild(Count), std::runtime_error);
}

TEST(trees, add_child) {
    test_add_child<un::BinaryTree<int>>();
    test_add_child<un::QuadTree<int>>();
    test_add_child<un::OcTree<int>>();
}