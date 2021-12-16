
#ifndef UNNECESSARYENGINE_TREE_H
#define UNNECESSARYENGINE_TREE_H

#include <vector>
#include <iterator>
#include <stack>

namespace un {
    template<typename T, typename std::size_t>
    concept
    bool TreeNavigable{
        return requires(const T& tree, std::size_t index){
            { tree.getChild(index) } -> T
        }
    };

    template<typename T>
    class Tree {
    private:
        T value;
        std::vector<Tree<T>> children;

    public:
        T& getChild(std::size_t index) {
            return children[index];
        }

        const T& getChild(std::size_t index) const {
            return children[index];
        }

        std::size_t addChild(T&& child) {
            std::size_t index = children.size();
            children.push_back(Tree(std::move(child)));
            return index;
        }

        class Iterator {
        private:
            std::stack<Tree<T>*> pending;
        public:
            Iterator(Tree<T>* root) {
                for (const auto& item : root->children) {

                }
            }

            void operator++() {
                Tree<T>* current = this->operator*();
                for (Tree<T>*& item : current->children) {
                    pending.emplace_back(item);
                }
            }

            Tree<T>* operator*() {
                return pending.front();
            }

            bool operator==(auto) {
                return pending.empty();
            }
        };


        Iterator begin() {

        }

        std::size_t end() {
            return 0;
        }

        template<typename T>
        un::Tree<T>* find(un::Tree<T>& root, std::size_t... indices) {
            un::Tree<T>* current = &root;
            for (std::size_t index : indices) {
                current = current->getChild(index);
            }
            return current;
        }

    public:
        explicit Tree(T&& value) : value(std::move(value)) {

        }
    };
}
#endif
