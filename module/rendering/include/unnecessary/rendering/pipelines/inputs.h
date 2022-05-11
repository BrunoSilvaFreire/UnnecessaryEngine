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

    class PipelineLayout {
    private:
        std::unordered_map<un::InputScope, un::DescriptorSetLayout> inputs;
    public:
        PipelineLayout(
            const std::initializer_list<decltype(inputs)::value_type>& layouts
        ) : inputs(layouts) {
        }
        PipelineLayout() = default;

        const std::unordered_map<un::InputScope, un::DescriptorSetLayout>& getDescriptorsLayout() const {
            return inputs;
        }

        std::unordered_map<un::InputScope, un::DescriptorSetLayout>& getDescriptorsLayout() {
            return inputs;
        }

    };
}
#endif //UNNECESSARYENGINE_INPUTS_H
