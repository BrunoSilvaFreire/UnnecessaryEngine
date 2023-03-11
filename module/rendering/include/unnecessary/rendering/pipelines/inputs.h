//
// Created by brunorbsf on 17/12/2021.
//

#ifndef UNNECESSARYENGINE_INPUTS_H
#define UNNECESSARYENGINE_INPUTS_H

#include <string>
#include <vector>
#include <unordered_map>
#include <unnecessary/rendering/layout/descriptor_set_layout.h>
#include <unnecessary/rendering/pipelines/input_scope.h>
#include <initializer_list>

namespace un {
    class pipeline_layout {
    private:
        std::unordered_map<input_scope, descriptor_set_layout> _inputs;

    public:
        pipeline_layout(
            const std::initializer_list<decltype(_inputs)::value_type>& layouts
        ) : _inputs(layouts) {
        }

        pipeline_layout() = default;

        const std::unordered_map<input_scope, descriptor_set_layout>&
        get_descriptors_layout() const {
            return _inputs;
        }

        std::unordered_map<input_scope, descriptor_set_layout>& get_descriptors_layout() {
            return _inputs;
        }
    };
}
#endif //UNNECESSARYENGINE_INPUTS_H
