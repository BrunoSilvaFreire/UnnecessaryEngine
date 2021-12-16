#ifndef UNNECESSARYENGINE_JOB_H
#define UNNECESSARYENGINE_JOB_H

#include <string>
#include <functional>

namespace un {

    typedef u32 JobHandle;

    template<typename T>
    using JobProvider = std::function<bool(T** jobPtr, JobHandle* id)>;

    template<typename T>
    using JobNotifier = std::function<void(T* jobPtr, JobHandle id)>;

    template<typename _Worker>
    class Job {
    private:
        std::string name = "Unnamed Job";
    public:
        typedef _Worker WorkerType;

        virtual void operator()(WorkerType* worker) = 0;

        const std::string& getName() const {
            return name;
        }

        void setName(const std::string& newName) {
            name = newName;
        }
    };
}
#endif
