#ifndef UNNECESSARYENGINE_GRAPH_RECORDER_H
#define UNNECESSARYENGINE_GRAPH_RECORDER_H

#include <unnecessary/jobs/job_graph.h>

namespace un {
    template<typename TJobSystem>
    class JobGraphRecorder {
    private:

    public:
        JobGraphRecorder(const TJobSystem& jobSystem) {
            jobSystem.getJobUnlocked() += [](const un::JobNode& node) {

            };
        }

    };
}
#endif
