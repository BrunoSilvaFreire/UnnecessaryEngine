#include <algorithm>
#include <utility>
#include <gtest/gtest.h>
#include <unnecessary/algorithms/n_tree.h>
#include <unnecessary/jobs/simple_jobs.h>
#include <unnecessary/jobs/dynamic_chain.h>
#include <unnecessary/jobs/job_system_builder.h>
#include <grapphs/tests/mazes.h>

un::quad_tree<int> generate_tree(int min = 0) {
    un::quad_tree<int> tree;
    int toAdd = std::max(
        static_cast<int>(rand() % un::quad_tree<int>::num_children()),
        min
    );
    for (std::size_t i = 0; i < toAdd; ++i) {
        tree.add_child(generate_tree());
    }
    return tree;
};

class process_node_job : public un::simple_job {
private:
    int _index;
    std::shared_ptr<un::quad_tree<int>> _node;

public:
    explicit process_node_job(
        int index,
        std::shared_ptr<un::quad_tree<int>> node
    ) : _index(index), node(std::move(node)) {
    }

    void operator()(worker_type* worker) override {
        LOG(INFO) << "Index " << _index << " was processed";
    }
};

TEST(jobs, dynamic_dispatch) {
    un::job_system_builder<un::simple_job_system> builder;
    builder.with_recorder();
    builder.with_logger();
    auto jobSystem = builder.build();
    auto tree = std::make_shared<un::quad_tree<int>>(generate_tree(4));
    int i = 0;
    std::unordered_map<un::job_handle, std::shared_ptr<un::quad_tree<int>>> dic;
    auto chain = un::create_dynamic_chain(
        jobSystem.get(),
        [&]<typename t_archetype>(
            un::job_handle handle,
            un::ptr<typename t_archetype::job_type> job,
            un::job_chain<un::simple_job_system>& subChain
        ) {
            const auto& children = dic[handle]->get_children();
            for (const auto& item : children) {
                if (item == nullptr) {
                    continue;
                }
                un::job_handle subHandle;
                subChain.immediately<process_node_job>(&subHandle, i, item);
                dic[subHandle] = item;
            }
        }
    );
    {
        un::job_chain<un::simple_job_system> jobs(jobSystem.get());
        un::job_handle firstHandle;
        jobs.immediately<process_node_job>(&firstHandle, i, tree);
        dic[firstHandle] = tree;
        chain.include(jobs);
    }
    chain.wait();
}
