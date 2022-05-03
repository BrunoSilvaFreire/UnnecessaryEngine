//
// Created by brunorbsf on 17/12/2021.
//

#ifndef UNNECESSARYENGINE_INPUTS_H
#define UNNECESSARYENGINE_INPUTS_H

#include <string>
#include <vector>
#include <unordered_map>
#include <unnecessary/strings.h>
#include <unnecessary/rendering/layout/descriptor_set_layout.h>
#include <initializer_list>

namespace un {

    enum InputScope {
        /**
         * This input is only set once, at the start of the frame
         */
        eGlobal,
        /**
         * This input is set everytime the pipeline is bound
         */
        ePipeline,
        /**
         * This input is set once per draw instance
         */
        eInstance
    };

    template<>
    std::string to_string(const un::InputScope& scope);


    namespace inputs {
        std::unordered_map<std::string, un::InputScope> const kStringIdsToInputScope = {
            std::make_pair("global", un::InputScope::eGlobal),
            std::make_pair("pipeline", un::InputScope::ePipeline),
            std::make_pair("instance", un::InputScope::eInstance)
        };

    }
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
