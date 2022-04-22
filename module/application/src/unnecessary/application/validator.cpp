#include <unnecessary/application/validator.h>

#include <utility>

namespace un {

    Solution::Solution(
        std::string description,
        std::function<void()> action
    ) : _description(std::move(description)),
        _action(std::move(action)) { }

    const std::string& Solution::getDescription() const {
        return _description;
    }

    const std::function<void()>& Solution::getAction() const {
        return _action;
    }

    Problem::Problem(std::string description) : _description(std::move(description)) { }

    void Problem::addSolution(Solution&& solution) {
        _solutions.push_back(std::move(solution));
    }

    const std::string& Problem::getDescription() const {
        return _description;
    }

    const std::vector<un::Solution>& Problem::getSolutions() const {
        return _solutions;
    }

    void Validator::addProblem(Problem&& problem) {
        _problems.push_back(std::move(problem));
    }

    Problem& Validator::addProblem(const std::string& problem) {
        return _problems.emplace_back(problem);
    }

    bool Validator::isValid() const {
        return _problems.empty();
    }

    const std::vector<un::Problem>& Validator::getProblems() const {
        return _problems;
    }
}