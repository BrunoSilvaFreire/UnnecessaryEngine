#ifndef UNNECESSARYENGINE_JOB_H
#define UNNECESSARYENGINE_JOB_H

#include <string>
#include <functional>
#include <unnecessary/def.h>

namespace un {
    using job_handle = u32;

    template<typename t_worker>
    class job {
    public:
        using worker_type = t_worker;

    protected:
        std::string _name = "Unnamed Job";

    public:
        job() = default;

        job(const job<worker_type>&) = delete;

        job(job<worker_type>&&) = delete;

        virtual ~job() = default;

        virtual void operator()(worker_type* worker) = 0;

        const std::string& get_name() const {
            return _name;
        }

        void set_name(const std::string& newName) {
            _name = newName;
        }
    };
}
#endif
