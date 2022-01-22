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
        std::thread _inner;
        std::function<void()> _block;

        void operator()();

    public:
        Thread(const std::function<void()>& block);

        Thread(const std::string& name, const std::function<void()>& block);

        void setName(const std::string& name);

        bool setAffinityMask(std::size_t mask);

    };
}
#endif
