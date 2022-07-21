#include <algorithm>
#include <utility>
#include <gtest/gtest.h>
#include <unnecessary/algorithms/n_tree.h>
#include <unnecessary/jobs/simple_jobs.h>
#include <unnecessary/jobs/dynamic_chain.h>
#include <unnecessary/jobs/job_system_builder.h>
#include <grapphs/tests/mazes.h>


un::QuadTree<int> generate_tree(int min = 0) {
    un::QuadTree<int> tree;
    int toAdd = std::max(
        static_cast<int>(rand() % un::QuadTree<int>::num_children()),
        min
    );
    for (std::size_t i = 0; i < toAdd; ++i) {
        tree.addChild(generate_tree());
    }
    return tree;
};

class ProcessNodeJob : public un::SimpleJob {
private:
    int index;
    std::shared_ptr<un::QuadTree<int>> node;
public:
    explicit ProcessNodeJob(
        int index,
        std::shared_ptr<un::QuadTree<int>> node
    ) : index(index), node(std::move(node)) { }

    void operator()(WorkerType* worker) override {
        LOG(INFO) << "Index " << index << " was processed";
    }
};

TEST(jobs, dynamic_dispatch) {
    un::JobSystemBuilder<un::SimpleJobSystem> builder;
    builder.withRecorder();
    builder.withLogger();
    auto jobSystem = builder.build();
    auto tree = std::make_shared<un::QuadTree<int>>(generate_tree(4));
    int i = 0;
    std::unordered_map<un::JobHandle, std::shared_ptr<un::QuadTree<int>>> dic;
    auto chain = un::create_dynamic_chain(
        jobSystem.get(),
        [&]<typename TArchetype>(
            un::JobHandle handle,
            un::ptr<typename TArchetype::JobType> job,
            un::JobChain<un::SimpleJobSystem>& subChain
        ) {
            const auto& children = dic[handle]->getChildren();
            for (const auto& item : children) {
                if (item == nullptr) {
                    continue;
                }
                un::JobHandle subHandle;
                subChain.immediately<ProcessNodeJob>(&subHandle, i, item);
                dic[subHandle] = item;
            }
        }
    );
    {
        un::JobChain<un::SimpleJobSystem> jobs(jobSystem.get());
        un::JobHandle firstHandle;
        jobs.immediately<ProcessNodeJob>(&firstHandle, i, tree);
        dic[firstHandle] = tree;
        chain.include(jobs);
    }
    chain.wait();
}