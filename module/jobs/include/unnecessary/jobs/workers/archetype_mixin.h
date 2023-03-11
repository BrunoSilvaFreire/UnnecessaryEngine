//
// Created by brunorbsf on 09/03/22.
//

#ifndef UNNECESSARYENGINE_ARCHETYPE_MIXIN_H
#define UNNECESSARYENGINE_ARCHETYPE_MIXIN_H

#include <unnecessary/jobs/workers/worker_pool.h>

namespace un {

    template<typename worker_type>
    class worker_chain;

    template<typename t_job_system>
    class job_chain;

    template<typename t_worker>
    class archetype_mixin {
    public:
        using worker_type = t_worker;
    protected:
        un::worker_pool<worker_type> _pool;
    public:
        archetype_mixin() : _pool() {

        }

    protected:
        /**
         * Dispatch a local handle to the worker pool.
         */
        void dispatch_local(job_handle handle) {
            _pool.dispatch(handle);
        }

        /**
         * Dispatch a set of local handles to the worker pool.
         */
        void dispatch_local(const std::set<job_handle>& handles) {
            _pool.dispatch(handles);
        }

    public:
        friend class worker_chain<worker_type>;

        template<typename>
        friend
        class job_chain;
    };
}
#endif
