#ifndef UNNECESSARYENGINE_JOB_DISPATCH_H
#define UNNECESSARYENGINE_JOB_DISPATCH_H

#include <set>
#include <unnecessary/jobs/job.h>
#include <unnecessary/misc/templates.h>

namespace un {
    class dispatch_batch {
    protected:
        std::set<un::job_handle> _batch;
    public:
        void dispatch(job_handle handle);

        void erase(job_handle handle);

        const std::set<job_handle>& get_batch() const;
    };

    template<typename t_worker>
    class dispatch_batch_mixin : public dispatch_batch {
    };

    template<typename ...archetypes>
    class job_dispatch_table : private dispatch_batch_mixin<archetypes> ... {
    public:
        template<typename t_worker>
        void dispatch(un::job_handle handle) {
            dispatch_batch_mixin<t_worker>::dispatch(handle);
        }

        template<typename t_worker>
        void erase(un::job_handle handle) {
            dispatch_batch_mixin<t_worker>::erase(handle);
        }

        template<typename t_worker>
        const std::set<un::job_handle>& get_batch() const {
            return dispatch_batch_mixin<t_worker>::get_batch();
        }
    };

}
#endif
