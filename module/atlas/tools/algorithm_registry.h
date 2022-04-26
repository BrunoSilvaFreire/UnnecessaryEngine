#ifndef UNNECESSARYENGINE_ALGORITHM_REGISTRY_H
#define UNNECESSARYENGINE_ALGORITHM_REGISTRY_H

#include <vector>
#include <packing_algorithm.h>
#include <unnecessary/misc/templates.h>
#include <algorithms/heuristics.h>
#include <algorithms/max_rectangles_algorithm.h>

namespace un::packer {
    class AlgorithmRegistry {
    private:
        std::vector<std::shared_ptr<un::packer::PackingAlgorithm>> algorithms;
    public:
        AlgorithmRegistry() {
            un::for_types<
                un::packer::MaxRectanglesAlgorithm<un::bottomLeftHeuristic>,
                un::packer::MaxRectanglesAlgorithm<un::bestLongSideFitHeuristic>,
                un::packer::MaxRectanglesAlgorithm<un::bestShortSideFitHeuristic>,
                un::packer::MaxRectanglesAlgorithm<un::bestAreaFitHeuristic>
            >(
                [&]<typename T>() {
                    algorithms.emplace_back(std::make_shared<T>());
                }
            );
        }

        const std::vector<std::shared_ptr<un::packer::PackingAlgorithm>>& getAlgorithms() const {
            return algorithms;
        }
    };
}
#endif