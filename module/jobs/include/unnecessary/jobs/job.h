#ifndef UNNECESSARYENGINE_JOB_H
#define UNNECESSARYENGINE_JOB_H

#include <string>
#include <functional>
#include <unnecessary/def.h>

namespace un {

    typedef u32 JobHandle;

    template<typename TWorker>
    class Job {
    public:
        typedef TWorker WorkerType;
    protected:
        std::string name = "Unnamed Job";
    public:
        Job() = default;

        Job(const Job<WorkerType>&) = delete;

        Job(Job<WorkerType>&&) = delete;

        virtual ~Job() = default;

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
