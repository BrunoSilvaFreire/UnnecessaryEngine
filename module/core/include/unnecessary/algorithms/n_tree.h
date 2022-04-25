#ifndef UNNECESSARYENGINE_N_TREE_H
#define UNNECESSARYENGINE_N_TREE_H

#include <array>
#include <optional>
#include <concepts>

namespace un {


    template<typename TData>
    concept TreeData = std::is_default_constructible<TData>();

    template<TreeData TData, std::size_t NChildren>
    class NTree {
    public:
        typedef NTree<TData, NChildren> TSelf;
        typedef std::shared_ptr<TSelf> TPointer;
        typedef std::array<TPointer, NChildren> TContainer;

        constexpr static std::size_t num_children() {
            return NChildren;
        }

    private:
        TData data;
        TContainer children;

        std::size_t findFirstFreeIndex() {
            for (std::size_t i = 0; i < std::numeric_limits<std::size_t>::max(); ++i) {
                if (children[i] == nullptr) {
                    return i;
                }
            }
            return std::numeric_limits<std::size_t>::max();
        }

    public:
        NTree() : data(), children() { };

        explicit NTree(TData&& data) : data(std::move(data)), children() { }

        explicit NTree(const TData& data) : data(data), children() { }

        TData getData() const {
            return data;
        }

        void setData(TData data) {
            NTree::data = data;
        }

        const TPointer& getChild(std::size_t i) const {
            return children[i];
        }

        TPointer& getChild(std::size_t i) {
            return children[i];
        }

        template<std::size_t Index>
        const TPointer& getChild() const {
            static_assert(Index < NChildren, "Index out of bounds.");
            return children[Index];
        }

        template<std::size_t Index>
        TPointer& getChild() {
            static_assert(Index < NChildren, "Index out of bounds.");
            return children[Index];
        }

        template<std::size_t Index>
        TPointer& setChild(TData&& childData) {
            return children[Index] = std::make_shared<TSelf>(std::move(childData));
        };

        template<std::size_t Index>
        TPointer& setChild(const TData& childData) {
            return children[Index] = std::make_shared<TSelf>(childData);
        };

        TPointer& setChild(std::size_t i, TData&& childData) {
            return children[i] = std::make_shared<TSelf>(std::move(childData));
        };

        TPointer& setChild(std::size_t i, const TData& childData) {
            return children[i] = std::make_shared<TSelf>(childData);
        };

        TPointer& addChild(const TData& child) {
            std::size_t firstFreeIndex = findFirstFreeIndex();
            if (firstFreeIndex >= NChildren) {
                throw std::runtime_error("No free children.");
            }
            return setChild(firstFreeIndex, child);
        }

        TPointer& addChild(TData&& child) {
            std::size_t firstFreeIndex = findFirstFreeIndex();
            if (firstFreeIndex >= NChildren) {
                throw std::runtime_error("No free children.");
            }
            return setChild(firstFreeIndex, std::move(child));
        }

        const std::array<TSelf, NChildren>& getChildren() const {
            return children;
        }

        std::array<TSelf, NChildren>& getChildren() {
            return children;
        }
    };

    template<typename TData>
    using BinaryTree = NTree<TData, 2>;

    template<typename TData>
    using QuadTree = NTree<TData, 4>;

    template<typename TData>
    using OcTree = NTree<TData, 8>;
}
#endif