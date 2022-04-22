
#ifndef UNNECESSARYENGINE_REQUIREMENTS_H
#define UNNECESSARYENGINE_REQUIREMENTS_H

#include <algorithm>
#include <memory>
#include <vector>
#include <concepts>
#include <unnecessary/application/validator.h>

namespace un {
    template<typename T>
    class Requirement {
    public:
        virtual void check(const T& value, un::Validator& validator) = 0;
    };

    template<typename A, typename B, B (* Transformer)(const A&)>

    class TransformingRequirement : public Requirement<A> {
    private:
        std::vector<std::shared_ptr<Requirement<B>>> children;
    public:

        template<typename E, typename ...Args>
        E& emplace(Args&& ... args) {
            std::shared_ptr<E> ptr = std::make_shared<E, Args&& ...>(std::forward<Args&& ...>(args...));
            children.push_back(ptr);
            return *ptr;
        }

        void check(const A& value, un::Validator& validator) override {
            const B transformed = Transformer(value);
            for (const std::shared_ptr<Requirement<B>>& ptr : children) {
                ptr->check(transformed, validator);
            }
        }
    };

    template<typename T>
    class CompositeRequirement : public Requirement<T> {
    private:
        std::vector<std::shared_ptr<Requirement<T>>> children;
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

        void check(const T& value, un::Validator& validator) override {
            std::for_each(
                children.begin(),
                children.end(),
                [&](const std::shared_ptr<Requirement<T>>& ptr) {
                    return ptr->check(value, validator);
                }
            );
        }
    };
}
#endif
