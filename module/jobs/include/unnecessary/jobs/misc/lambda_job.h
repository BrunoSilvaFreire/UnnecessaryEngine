#ifndef UNNECESSARYENGINE_LAMBDA_JOB_H
#define UNNECESSARYENGINE_LAMBDA_JOB_H

#include <unnecessary/jobs/job_system.h>

namespace un {

    template<typename _Worker = JobWorker>
    class LambdaJob : public Job<_Worker> {
    public:
        typedef std::function<void(typename un::Job<_Worker>::WorkerType* worker)> Callback;
        typedef std::function<void()> VoidCallback;
    private:
        Callback callback;
        VoidCallback voidCallback;
    public:
        LambdaJob(const Callback& callback) : callback(callback), voidCallback(nullptr) { }


        LambdaJob(const VoidCallback& callback) : voidCallback(callback), callback() {

        };

        void operator()(typename un::Job<_Worker>::WorkerType* worker) override {
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
