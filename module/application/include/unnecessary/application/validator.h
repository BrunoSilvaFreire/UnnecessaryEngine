#ifndef UNNECESSARYENGINE_VALIDATOR_H
#define UNNECESSARYENGINE_VALIDATOR_H

#include <utility>
#include <vector>
#include <string>
#include <functional>

namespace un {
    class solution {
    private:
        std::string _description;
        std::function<void()> _action;

    public:
        solution(
            std::string description,
            std::function<void()> action
        );

        const std::string& get_description() const;

        const std::function<void()>& get_action() const;
    };

    class problem {
    private:
        std::string _description;
        std::vector<solution> _solutions;

    public:
        explicit problem(std::string description);

        void add_solution(solution&& solution);

        const std::string& get_description() const;

        const std::vector<solution>& get_solutions() const;
    };

    class validator {
    private:
        std::vector<problem> _problems;

    public:
        void add_problem(problem&& problem);

        const std::vector<problem>& get_problems() const;

        bool is_valid() const;

        problem& add_problem(const std::string& problem);
    };
}
#endif
