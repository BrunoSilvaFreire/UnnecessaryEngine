#include <unnecessary/jobs/thread.h>
#include <pthread.h>
#include <cstring>
#include <iostream>
#include "unnecessary/logging.h"

namespace un {

    struct ThreadPlatformBridge {
    public:
        pthread_t _pthread;
        char* _nameStorage = nullptr;
    };

    un::void_ptr unnecessary_unix_proc(un::void_ptr ptr) {
        auto* thread = reinterpret_cast<un::Thread*>(ptr);

        const std::string& name = thread->_params.getName();
        if (!name.empty()) {
            const std::size_t kMaxLength = 16;
            ThreadPlatformBridge* pBridge = thread->_bridge;
            const char* pName;
            if (name.size() >= kMaxLength) {
                pBridge->_nameStorage = new char[kMaxLength];
                std::memcpy(pBridge->_nameStorage, name.data(), kMaxLength - 1);
                pName = pBridge->_nameStorage;
            } else {
                pName = name.c_str();
            }
            int statusCode = pthread_setname_np(pBridge->_pthread, pName);
            if (statusCode != 0) {
                if (statusCode == ERANGE) {
                    throw std::runtime_error("Unable to set thread name");
                }
            }
        }

        thread->operator()();
        return nullptr;
    }

    void Thread::start() {
        _bridge = new un::ThreadPlatformBridge();
        pthread_t* pThread = &_bridge->_pthread;
        pthread_attr_t attr{};
        pthread_attr_init(&attr);
        //pthread_attr_setstacksize(&attr, _params.getStackSize());

        size_t core = _params.getCore();
        if (core != un::ThreadParams::kAnyCore) {
            cpu_set_t set;
            CPU_ZERO(&set);
            CPU_SET(core, &set);
            pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &set);
        }

        pthread_create(pThread, &attr, &unnecessary_unix_proc, this);
    }

    void Thread::join() {

        std::unique_lock<std::mutex> lock(_dataMutex);
        if (_alive) {
            pthread_join(_bridge->_pthread, nullptr);
        }
    }
}