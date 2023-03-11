#ifndef UNNECESSARYENGINE_ALGORITHM_REGISTRY_H
#define UNNECESSARYENGINE_ALGORITHM_REGISTRY_H

#include <vector>
#include <packing_algorithm.h>
#include <unnecessary/misc/templates.h>
#include <algorithms/heuristics.h>
#include <algorithms/max_rectangles_algorithm.h>

namespace un::packer {
    class algorithm_registry {
    private:
        std::vector<std::shared_ptr<packing_algorithm>> _algorithms;

    public:
        algorithm_registry() {
            un::for_types<
                max_rectangles_algorithm<bottom_left_heuristic>,
                max_rectangles_algorithm<best_long_side_fit_heuristic>,
                max_rectangles_algorithm<best_short_side_fit_heuristic>,
                max_rectangles_algorithm<best_area_fit_heuristic>
            >(
                [&]<typename T>() {
                    _algorithms.emplace_back(std::make_shared<T>());
                }
            );
        }

        const std::vector<std::shared_ptr<packing_algorithm>>& get_algorithms() const {
            return _algorithms;
        }
    };
}
#endif
