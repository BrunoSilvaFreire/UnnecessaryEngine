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
    struct thread_params {
    private:
        std::string _name;
        std::size_t _core;
        std::size_t _stackSize;

    public:
        static const std::size_t k_any_core = std::numeric_limits<std::size_t>::max();
        static const std::size_t k_default_stack_size = 4096;

        explicit thread_params(std::string name, std::size_t core = k_any_core);

        const std::string& get_name() const;

        size_t get_core() const;

        void set_stack_size(size_t stackSize);

        size_t get_stack_size() const;
    };

    struct thread_platform_bridge;

    class thread {
    private:
        std::function<void()> _block;
        std::mutex _dataMutex;
        bool _alive;
        thread_params _params;
        ptr<thread_platform_bridge> _bridge;

        void operator()();

        void set_thread_name(const std::string& name);

        bool set_thread_core(u32 core);

    public:
        explicit thread(const std::function<void()>& block);

        explicit thread(thread_params name, const std::function<void()>& block);

        thread() = delete;

        thread(const thread&) = delete;

        void start();

        void join();

#if UN_PLATFORM_WINDOWS

        friend DWORD WINAPI unnecessary_thread_proc(_In_ LPVOID lpParameter);
#elif UN_PLATFORM_UNIX

        friend un::void_ptr unnecessary_unix_proc(un::void_ptr ptr);

#endif
    };
}
#endif
