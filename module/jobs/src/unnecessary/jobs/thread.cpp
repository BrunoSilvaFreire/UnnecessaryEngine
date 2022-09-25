#include <unnecessary/jobs/thread.h>
#include <unnecessary/logging.h>
#include <mutex>
#include <utility>

namespace un {

    Thread::Thread(
        const std::function<void()>& block
    ) : Thread(un::ThreadParams("UnnecessaryThread"), block) {
    }

    Thread::Thread(
        un::ThreadParams parameters,
        const std::function<void()>& block
    ) : _block(block),
        _alive(false),
        _params(std::move(parameters)),
        _nativeHandle(nullptr) {
        if (block == nullptr) {
            throw std::runtime_error("Thread created with empty block.");
        }
    }

    void Thread::operator()() {
        std::unique_lock<std::mutex> lock(_dataMutex);
        _alive = true;
        _block();
        _alive = false;
    }

    bool Thread::isAlive() {
        std::unique_lock<std::mutex> lock(_dataMutex);
        return _alive;
    }

    ThreadParams::ThreadParams(
        std::string name,
        size_t core
    ) : _name(std::move(name)),
        _core(core),
        _stackSize(kDefaultStackSize) {

    }

    const std::string& ThreadParams::getName() const {
        return _name;
    }

    size_t ThreadParams::getCore() const {
        return _core;
    }

    size_t ThreadParams::getStackSize() const {
        return _stackSize;
    }

    void ThreadParams::setStackSize(size_t stackSize) {
        _stackSize = stackSize;
    }
}