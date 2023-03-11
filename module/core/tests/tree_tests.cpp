#include <gtest/gtest.h>
#include <unnecessary/algorithms/n_tree.h>

TEST(trees, assignment_and_access) {
    un::quad_tree<int> tree;
    constexpr std::size_t index = 0;
    un::quad_tree<int>::pointer_type& ptr = tree.get_child<index>();
    ASSERT_EQ(ptr, nullptr);
    ptr = tree.set_child<index>(5);
    ASSERT_NE(ptr, nullptr);
    ASSERT_EQ(ptr->get_data(), 5);
}

template<typename TTree>
void test_add_child() {
    TTree tree;
    constexpr std::size_t count = TTree::num_children();
    for (int i = 0; i < count; ++i) {
        ASSERT_NO_THROW(tree.add_child(i));
    }
    ASSERT_THROW(tree.add_child(count), std::runtime_error);
}

TEST(trees, add_child) {
    test_add_child<un::binary_tree<int>>();
    test_add_child<un::quad_tree<int>>();
    test_add_child<un::oc_tree<int>>();
}
