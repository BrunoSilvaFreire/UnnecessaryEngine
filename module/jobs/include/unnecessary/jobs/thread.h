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
        std::string name;
        u32 core;

        void operator()();

        void setThreadName(const std::string& name);

        bool setThreadCore(u32 core);

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
