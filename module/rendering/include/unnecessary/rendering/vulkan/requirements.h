
#ifndef UNNECESSARYENGINE_REQUIREMENTS_H
#define UNNECESSARYENGINE_REQUIREMENTS_H

#include <algorithm>
#include <memory>
#include <vector>
#include <concepts>
#include <unnecessary/application/validator.h>

namespace un {
    template<typename T>
    class requirement {
    public:
        virtual void check(const T& value, un::validator& validator) = 0;
    };

    template<typename A, typename B, B (* Transformer)(const A&)>

    class transforming_requirement : public requirement<A> {
    private:
        std::vector<std::shared_ptr<requirement<B>>> _children;
    public:

        template<typename E, typename ...Args>
        E& emplace(Args&& ... args) {
            std::shared_ptr<E> ptr = std::make_shared<
                E,
                Args&& ...
            >(std::forward<Args&& ...>(args...));
            _children.push_back(ptr);
            return *ptr;
        }

        void check(const A& value, un::validator& validator) override {
            const B transformed = Transformer(value);
            for (const std::shared_ptr<requirement<B>>& ptr : _children) {
                ptr->check(transformed, validator);
            }
        }
    };

    template<typename T>
    class composite_requirement : public requirement<T> {
    private:
        std::vector<std::shared_ptr<requirement<T>>> _children;
    public:
        template<typename E, typename ...Args>
        E& emplace(Args&& ... args) {
            std::shared_ptr<E> ptr = std::make_shared<E, Args&& ...>(
                std::forward<Args&& ...>(
                    args...
                ));
            _children.push_back(ptr);
            return *ptr;
        }

        template<typename E>
        E& emplace() {
            std::shared_ptr<E> ptr = std::make_shared<E>();
            _children.push_back(ptr);
            return *ptr;
        }

        void check(const T& value, un::validator& validator) override {
            std::for_each(
                _children.begin(),
                _children.end(),
                [&](const std::shared_ptr<requirement<T>>& ptr) {
                    return ptr->check(value, validator);
                }
            );
        }
    };
}
#endif
