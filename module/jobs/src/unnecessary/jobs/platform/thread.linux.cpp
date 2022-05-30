#include <unnecessary/jobs/thread.h>
#include <pthread.h>

namespace un {
    bool Thread::setThreadCore(u32 core) {
        cpu_set_t cpu;
        CPU_ZERO(&cpu);
        CPU_SET(core, &cpu);
//        int status = pthread_setaffinity_np(
//            _inner.native_handle(),
//            sizeof(cpu_set_t),
//            &cpu
//        );
//        return status == 0;
        return false;
    }

    void Thread::setThreadName(const std::string& name) {
        pthread_setname_np(
            _inner.native_handle(),
            name.c_str()
        );
    }

}