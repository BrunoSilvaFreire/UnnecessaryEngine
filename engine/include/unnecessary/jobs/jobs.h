#ifndef UNNECESSARYENGINE_JOBSYSTEM_H
#define UNNECESSARYENGINE_JOBSYSTEM_H

#include <queue>
#include <unnecessary/def.h>
#include <boost/graph/adjacency_list.hpp>

namespace un {
    class Job {
        virtual explicit operator void() = 0;
    };

    class JobSystem {
    public:
        class Node {
        private:
        };

    private:
        boost::adjacency_list<Node, boost::no_property, boost::no_property> tasks;
    public:
        JobSystem(u32 nWorkerThreads) {

        }
    };
}
#endif
