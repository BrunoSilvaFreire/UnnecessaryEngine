#include <unnecessary/jobs/thread.h>

//Unsupported
namespace un {
    bool Thread::setCore(u32 core) {
        return false;
    }

    void Thread::setName(const std::string& name) {
    }
}