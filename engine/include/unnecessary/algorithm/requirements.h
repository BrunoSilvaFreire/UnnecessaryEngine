
#ifndef UNNECESSARYENGINE_REQUIREMENTS_H
#define UNNECESSARYENGINE_REQUIREMENTS_H

#include <algorithm>
#include <memory>
#include <vector>
#include <concepts>

namespace un {
    template<typename T>
    class ARequirement {
    public:
        virtual bool isMet(const T& value) = 0;
    };

    template<typename A, typename B, B (* Transformer)(const A&)>

    class TransformingRequirement : public ARequirement<A> {
    private:
        std::vector<std::shared_ptr<ARequirement<B>>> children;
    public:

        template<typename E, typename ...Args>
        E& emplace(Args&& ... args) {
            std::shared_ptr<E> ptr = std::make_shared<E, Args&& ...>(
                std::forward<Args&& ...>(
                    args...
                ));
            children.push_back(ptr);
            return *ptr;
        }

        bool isMet(const A& value) override {
            const B transformed = Transformer(value);
            return std::all_of(
                children.begin(),
                children.end(),
                [&](const std::shared_ptr<ARequirement<B>>& ptr) {
                    return ptr->isMet(transformed);
                }
            );
        }
    };

    template<typename T>
    class CompositeRequirement : public ARequirement<T> {
    private:
        std::vector<std::shared_ptr<ARequirement<T>>> children;
    public:
        template<typename E, typename ...Args>
        E& emplace(Args&& ... args) {
            std::shared_ptr<E> ptr = std::make_shared<E, Args&& ...>(
                std::forward<Args&& ...>(
                    args...
                ));
            children.push_back(ptr);
            return *ptr;
        }

        template<typename E>
        E& emplace() {
            std::shared_ptr<E> ptr = std::make_shared<E>();
            children.push_back(ptr);
            return *ptr;
        }

        bool isMet(const T& value) override {
            return std::all_of(
                children.begin(),
                children.end(),
                [&](const std::shared_ptr<ARequirement<T>>& ptr) {
                    return ptr->isMet(value);
                }
            );
        }
    };
}
#endif
