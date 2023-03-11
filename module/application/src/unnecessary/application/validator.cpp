#include <unnecessary/application/validator.h>

#include <utility>

namespace un {
    solution::solution(
        std::string description,
        std::function<void()> action
    ) : _description(std::move(description)),
        _action(std::move(action)) {
    }

    const std::string& solution::get_description() const {
        return _description;
    }

    const std::function<void()>& solution::get_action() const {
        return _action;
    }

    problem::problem(std::string description) : _description(std::move(description)) {
    }

    void problem::add_solution(solution&& solution) {
        _solutions.push_back(std::move(solution));
    }

    const std::string& problem::get_description() const {
        return _description;
    }

    const std::vector<solution>& problem::get_solutions() const {
        return _solutions;
    }

    void validator::add_problem(problem&& problem) {
        _problems.push_back(std::move(problem));
    }

    problem& validator::add_problem(const std::string& problem) {
        return _problems.emplace_back(problem);
    }

    bool validator::is_valid() const {
        return _problems.empty();
    }

    const std::vector<problem>& validator::get_problems() const {
        return _problems;
    }
}
