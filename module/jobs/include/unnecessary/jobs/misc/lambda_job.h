#ifndef UNNECESSARYENGINE_LAMBDA_JOB_H
#define UNNECESSARYENGINE_LAMBDA_JOB_H

#include <functional>
#include <utility>
#include <unnecessary/jobs/job_system.h>
#include <unnecessary/jobs/simple_jobs.h>

namespace un {
    template<typename worker_type = job_worker>
    class lambda_job : public job<worker_type> {
    public:
        using callback = std::function<void(typename job<worker_type>::worker_type* worker)>;
        using void_callback = std::function<void()>;

    private:
        callback _callback;
        void_callback _voidCallback;

    public:
        explicit lambda_job(const callback& callback)
            : _callback(callback), _voidCallback(nullptr) {
        }

        explicit lambda_job(void_callback callback)
            : _callback(), _voidCallback(std::move(callback)) {
        };

        void operator()(typename job<worker_type>::worker_type* worker) override {
            if (_callback != nullptr) {
                _callback(worker);
            }
            if (_voidCallback != nullptr) {
                _voidCallback();
            }
        }
    };
}
#endif
