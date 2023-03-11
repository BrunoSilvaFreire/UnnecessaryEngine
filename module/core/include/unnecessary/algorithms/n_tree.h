#ifndef UNNECESSARYENGINE_N_TREE_H
#define UNNECESSARYENGINE_N_TREE_H

#include <array>
#include <optional>
#include <concepts>

namespace un {
    template<typename data_type>
    concept is_tree_data = std::is_default_constructible_v<data_type>();

    template<typename t_data, std::size_t n_children>
    //#ifdef __cpp_concepts
    //        requires un::is_tree_data<t_data>
    //#endif
    class n_tree {
    public:
        using data_type = t_data;
        using self_type = n_tree<data_type, n_children>;
        using pointer_type = std::shared_ptr<self_type>;
        using container_type = std::array<pointer_type, n_children>;

        constexpr static std::size_t num_children() {
            return n_children;
        }

    private:
        data_type _data;
        container_type _children;

        std::size_t find_first_free_index() {
            for (std::size_t i = 0; i < n_children; ++i) {
                if (_children[i] == nullptr) {
                    return i;
                }
            }
            return std::numeric_limits<std::size_t>::max();
        }

    public:
        n_tree() : _data(), _children() {
        };

        explicit n_tree(data_type&& data) : _data(std::move(data)), _children() {
        }

        explicit n_tree(const data_type& data) : _data(data), _children() {
        }

        data_type get_data() const {
            return _data;
        }

        void set_data(data_type data) {
            _data = data;
        }

        const pointer_type& get_child(std::size_t i) const {
            return _children[i];
        }

        pointer_type& get_child(std::size_t i) {
            return _children[i];
        }

        template<std::size_t Index>
        const pointer_type& get_child() const {
            static_assert(Index < n_children, "Index out of bounds.");
            return _children[Index];
        }

        template<std::size_t Index>
        pointer_type& get_child() {
            static_assert(Index < n_children, "Index out of bounds.");
            return _children[Index];
        }

        template<std::size_t Index>
        pointer_type& set_child(data_type&& childData) {
            return _children[Index] = std::make_shared<self_type>(std::move(childData));
        };

        template<std::size_t Index>
        pointer_type& set_child(const data_type& childData) {
            return _children[Index] = std::make_shared<self_type>(childData);
        };

        template<std::size_t Index>
        pointer_type& set_child(const pointer_type& childData) {
            return _children[Index] = childData;
        };

        template<std::size_t Index>
        pointer_type& set_child(const self_type& childData) {
            return _children[Index] = std::make_shared<self_type>(childData);
        };

        pointer_type& set_child(std::size_t i, data_type&& childData) {
            return _children[i] = std::make_shared<self_type>(std::move(childData));
        };

        pointer_type& set_child(std::size_t i, const data_type& childData) {
            return _children[i] = std::make_shared<self_type>(childData);
        };

        pointer_type& set_child(std::size_t i, const pointer_type& childData) {
            return _children[i] = childData;
        };

        pointer_type& set_child(std::size_t i, const self_type& childData) {
            return _children[i] = std::make_shared<self_type>(childData);
        };

        pointer_type& add_child(const data_type& child) {
            std::size_t firstFreeIndex = find_first_free_index();
            if (firstFreeIndex >= n_children) {
                throw std::runtime_error("No free children.");
            }
            return set_child(firstFreeIndex, child);
        }

        pointer_type& add_child(data_type&& child) {
            std::size_t firstFreeIndex = find_first_free_index();
            if (firstFreeIndex >= n_children) {
                throw std::runtime_error("No free children.");
            }
            return set_child(firstFreeIndex, std::move(child));
        }

        pointer_type& add_child(const pointer_type& child) {
            std::size_t firstFreeIndex = find_first_free_index();
            if (firstFreeIndex >= n_children) {
                throw std::runtime_error("No free children.");
            }
            return set_child(firstFreeIndex, child);
        }

        pointer_type& add_child(const self_type& child) {
            std::size_t firstFreeIndex = find_first_free_index();
            if (firstFreeIndex >= n_children) {
                throw std::runtime_error("No free children.");
            }
            return set_child(firstFreeIndex, child);
        }

        const std::array<pointer_type, n_children>& get_children() const {
            return _children;
        }

        std::array<pointer_type, n_children>& get_children() {
            return _children;
        }
    };

    template<typename TData>
    using binary_tree = n_tree<TData, 2>;

    template<typename TData>
    using quad_tree = n_tree<TData, 4>;

    template<typename TData>
    using oc_tree = n_tree<TData, 8>;
}
#endif
