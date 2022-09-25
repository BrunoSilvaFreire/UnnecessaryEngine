//
// Created by bruno on 20/01/2022.
//

#ifndef UNNECESSARYENGINE_THREAD_H
#define UNNECESSARYENGINE_THREAD_H

#include <thread>
#include <functional>
#include <mutex>
#include <unnecessary/def.h>

#if UN_PLATFORM_WINDOWS

#include <Windows.h>

#endif

namespace un {
    struct ThreadParams {
    private:
        std::string _name;
        std::size_t _core;
        std::size_t _stackSize;
    public:
        static const std::size_t kDefaultStackSize = 4096;

        ThreadParams(std::string  name, std::size_t core = -1);

        const std::string& getName() const;

        size_t getCore() const;

        void setStackSize(size_t stackSize);

        size_t getStackSize() const;
    };

    class Thread {
    private:
        std::function<void()> _block;
        std::mutex _dataMutex;
        bool _alive;
        un::ThreadParams _params;
        un::void_ptr _nativeHandle;

        void operator()();

        void setThreadName(const std::string& name);

        bool setThreadCore(u32 core);

    public:
        explicit Thread(const std::function<void()>& block);

        explicit Thread(un::ThreadParams name, const std::function<void()>& block);

        Thread() = delete;

        Thread(const Thread&) = delete;

        void start();

        void join();

        bool isAlive();

#if UN_PLATFORM_WINDOWS

        friend DWORD WINAPI UnnecessaryThreadProc(_In_ LPVOID lpParameter);

#endif
    };
}
#endif
