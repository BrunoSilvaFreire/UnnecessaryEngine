//
// Created by bruno on 20/01/2022.
//

#ifndef UNNECESSARYENGINE_THREAD_H
#define UNNECESSARYENGINE_THREAD_H

#include <thread>
#include <functional>

namespace un {
    class Thread {
    private:
        std::function<void()> _block;
        std::thread _inner;

        void operator()();

    public:
        Thread(const std::function<void()>& block);

        Thread(const std::string& name, const std::function<void()>& block);

        Thread() = delete;

        Thread(const Thread& cpy) = delete;

        void setName(const std::string& name);

        bool setAffinityMask(std::size_t mask);

    };
}
#endif
