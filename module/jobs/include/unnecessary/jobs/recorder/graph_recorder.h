#ifndef UNNECESSARYENGINE_GRAPH_RECORDER_H
#define UNNECESSARYENGINE_GRAPH_RECORDER_H

#include <unnecessary/jobs/job_graph.h>

namespace un {
    template<typename t_job_system>
    class job_graph_recorder {
    private:

    public:
        job_graph_recorder(const t_job_system& jobSystem) {
            jobSystem.getJobUnlocked() += [](const un::job_node& node) {

            };
        }

    };
}
#endif
