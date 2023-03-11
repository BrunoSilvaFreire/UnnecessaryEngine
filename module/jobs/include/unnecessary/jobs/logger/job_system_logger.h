#ifndef UNNECESSARYENGINE_JOB_SYSTEM_LOGGER_H
#define UNNECESSARYENGINE_JOB_SYSTEM_LOGGER_H

#include <unnecessary/jobs/job_system.h>
#include <unnecessary/logging.h>
#include <unnecessary/misc/pretty_print.h>

namespace un {
    template<typename t_worker>
    class worker_logger {
    public:
        using worker_type = t_worker;

    private:
        worker_type* _worker;

        std::string worker_header() {
            std::stringstream stream;

            stream << termcolor::colorize << PURPLE(_worker->get_name());
            return stream.str();
        }

    public:
        worker_type* get_worker() const {
            return _worker;
        }

        worker_logger(worker_logger&&) = delete;

        worker_logger(const worker_logger&) = delete;

        explicit worker_logger(worker_type* type) : _worker(type) {
            using job_type = typename worker_type::job_type;
            _worker->on_exited() += [this]() {
                auto message = un::message("JobSystem");
                message.text() << worker_header() << " " << RED("exited") << ".";
            };
            _worker->on_sleeping() += [this]() {
                auto message = un::message("JobSystem");
                message.text() << worker_header() << " is " << YELLOW("sleeping") << ".";
            };
            _worker->on_awaken() += [this]() {
                auto message = un::message("JobSystem");
                message.text() << worker_header() << " has " << GREEN("awaken") << ".";
            };
            _worker->on_started() += [this]() {
                auto message = un::message("JobSystem");
                message.text() << worker_header() << " has " << GREEN("started") << " .";
            };
            _worker->on_fetched() += [this](job_type* job, job_handle handle) {
                auto message = un::message("JobSystem");
                message.text() << worker_header() << " " << YELLOW("fetched") << " job "
                               << GREEN(handle) << " ("
                               << GREEN(job->get_name()) << ").";
            };
            _worker->on_executed() += [this](job_type* job, job_handle handle) {
                auto message = un::message("JobSystem");
                message.text() << worker_header() << " " << GREEN("executed") << " job "
                               << GREEN(handle) << " ("
                               << GREEN(job->get_name())
                               << ").";
            };
            _worker->on_enqueued() += [this](job_type* job, job_handle handle) {
                auto message = un::message("JobSystem");
                message.text() << worker_header() << " " << PURPLE("enqueued") << " job "
                               << GREEN(handle) << " ("
                               << GREEN(job->get_name())
                               << ").";
            };
        }
    };

    template<typename worker_type>
    class logger_pool {
    private:
        std::vector<std::unique_ptr<worker_logger<worker_type>>> _loggers;

    public:
        void add_logger(worker_type* worker) {
            _loggers.emplace_back(std::make_unique<worker_logger<worker_type>>(worker));
        }
    };

    template<typename t_job_system>
    class job_system_logger : public t_job_system::extension_type {
    public:
        using logger_pool_tuple = typename t_job_system::template archetype_tuple<logger_pool>;

    private:
        logger_pool_tuple _loggers;

    public:
        void apply(typename t_job_system::extension_type::target_type& jobSystem) override {
            jobSystem.for_each_worker(
                [&]<typename t_archetype, std::size_t TArchetypeIndex>(t_archetype* worker) {
                    std::get<TArchetypeIndex>(_loggers).add_logger(worker);
                }
            );
            jobSystem.on_unlock_failed() += [
                this,
                &jobSystem
            ](job_handle handle, const job_node& node) {
                const dependency_graph<job_node>& jobGraph = jobSystem.get_job_graph();
                rich_message root = message("JobSystem");
                auto& header = root.text();
                auto& content = root.map();
                auto& jobName = content.text("job");
                auto& jobHandle = content.text("handle");
                auto& description = root.map();
                auto& missingJobsMsg = content.tree("dependencies");
                for (auto dependencyIndex : jobGraph.dependencies_of(handle)) {
                    const job_node* dependencyNode = jobGraph[dependencyIndex];
                    t_job_system::for_each_archetype(
                        [&]<typename t_archetype, std::size_t TArchetypeIndex>() {
                            if (dependencyNode->archetypeIndex != TArchetypeIndex) {
                                return;
                            }
                            auto* job = jobSystem.template get_worker_pool<t_archetype>()
                                                 .get_job(dependencyNode->poolLocalIndex);
                            auto& tree = missingJobsMsg.map();
                            text_message& entry = tree.text("name");
                            if (job == nullptr) {
                                entry << RED("Unknown Job (nullptr)");
                            }
                            else {
                                entry << job->get_name();
                            }

                            tree.text("handle (poolLocalIndex)") << dependencyNode->poolLocalIndex;
                            tree.text("archetype") << un::type_name_of<t_archetype>();
                            tree.text("archetypeIndex") << dependencyNode->archetypeIndex;
                        }
                    );
                }
                t_job_system::for_each_archetype(
                    [&]<typename t_archetype, std::size_t TArchetypeIndex>() {
                        if (node.archetypeIndex != TArchetypeIndex) {
                            return;
                        }

                        auto* job = jobSystem.template get_worker_pool<t_archetype>()
                                             .get_job(node.poolLocalIndex);
                        jobName << GREEN(job->get_name());
                        jobHandle << node.poolLocalIndex;
                        header
                            << "A job failed to be unlocked because of unsatisfied dependencies ("
                            << RED(missingJobsMsg.num_children() << " total") << ")";
                    }
                );
            };
        }
    };
}
#endif
