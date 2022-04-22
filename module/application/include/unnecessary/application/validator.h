#ifndef UNNECESSARYENGINE_VALIDATOR_H
#define UNNECESSARYENGINE_VALIDATOR_H

#include <utility>
#include <vector>
#include <string>
#include <functional>

namespace un {
    class Solution {
    private:
        std::string _description;
        std::function<void()> _action;
    public:
        Solution(
            std::string description,
            std::function<void()> action
        );

        const std::string& getDescription() const;

        const std::function<void()>& getAction() const;
    };

    class Problem {
    private:
        std::string _description;
        std::vector<un::Solution> _solutions;
    public:
        explicit Problem(std::string description);

        void addSolution(un::Solution&& solution);

        const std::string& getDescription() const;

        const std::vector<un::Solution>& getSolutions() const;
    };

    class Validator {
    private:
        std::vector<un::Problem> _problems;
    public:
        void addProblem(un::Problem&& problem);

        const std::vector<un::Problem>& getProblems() const;

        bool isValid() const;

        Problem& addProblem(const std::string& problem);
    };
}
#endif