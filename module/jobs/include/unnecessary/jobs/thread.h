//
// Created by bruno on 20/01/2022.
//

#ifndef UNNECESSARYENGINE_THREAD_H
#define UNNECESSARYENGINE_THREAD_H

#include <thread>
#include <functional>
#include <mutex>
#include <unnecessary/def.h>

namespace un {
    class Thread {
    private:
        std::function<void()> _block;
        std::thread _inner;
        std::mutex _dataMutex;
        bool _alive;

        void operator()();

        void setAliveThreadName(const std::string& name);

        bool setAliveThreadCore(u32 core);

    public:
        explicit Thread(const std::function<void()>& block);

        Thread(const std::string& name, const std::function<void()>& block);

        Thread() = delete;

        Thread(const Thread& cpy) = delete;

        void setName(const std::string& name);

        bool setCore(u32 core);

        void start();

        void join();

        bool isAlive();
    };
}
#endif
