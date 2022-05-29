#ifndef UNNECESSARYENGINE_LAMBDA_JOB_H
#define UNNECESSARYENGINE_LAMBDA_JOB_H

#include <functional>
#include <utility>
#include <unnecessary/jobs/job_system.h>
#include <unnecessary/jobs/simple_jobs.h>

namespace un {

    template<typename TWorker = un::JobWorker>
    class LambdaJob : public Job<TWorker> {
    public:
        typedef std::function<void(typename un::Job<TWorker>::WorkerType* worker)> Callback;
        typedef std::function<void()> VoidCallback;
    private:
        Callback callback;
        VoidCallback voidCallback;
    public:
        LambdaJob(const Callback& callback) : callback(callback), voidCallback(nullptr) { }


        LambdaJob(VoidCallback callback) : voidCallback(std::move(callback)), callback() {

        };

        void operator()(typename un::Job<TWorker>::WorkerType* worker) override {
            if (callback != nullptr) {
                callback(worker);
            }
            if (voidCallback != nullptr) {
                voidCallback();
            }
        }
    };


}
#endif
