#include <unnecessary/jobs/thread.h>

//Unsupported
namespace un {
    bool thread::set_thread_core(u32 core) {
        return false;
    }

    void thread::set_thread_name(const std::string& name) {
    }

}