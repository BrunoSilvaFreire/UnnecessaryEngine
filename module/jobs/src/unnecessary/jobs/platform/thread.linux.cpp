#include <unnecessary/jobs/thread.h>
#include <pthread.h>

namespace un {
    un::void_ptr unnecessary_unix_proc(un::void_ptr ptr) {
        reinterpret_cast<un::Thread*>(ptr)->operator()();
        return nullptr;
    }

    void Thread::start() {
        pthread_t* pThread;
        _nativeHandle = pThread = new pthread_t();
        pthread_attr_t attr{};
        pthread_attr_init(&attr);
        pthread_attr_setstacksize(&attr, _params.getStackSize());

        size_t core = _params.getCore();
        if (core != un::ThreadParams::kAnyCore) {
            cpu_set_t set;
            CPU_ZERO(&set);
            CPU_SET(core, &set);
            pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &set);
        }

        int threadId = pthread_create(pThread, &attr, &unnecessary_unix_proc, this);
        const std::string& name = _params.getName();
        if (!name.empty()) {
            pthread_setname_np(*pThread, name.c_str());
        }

    }

    void Thread::join() {
        pthread_join(*reinterpret_cast<pthread_t*>(_nativeHandle), nullptr);
    }
}