//
// Created by brunorbsf on 17/12/2021.
//

#ifndef UNNECESSARYENGINE_INPUTS_H
#define UNNECESSARYENGINE_INPUTS_H

#include <string>
#include <vector>
#include <unordered_map>
#include <unnecessary/strings.h>

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
        std::unordered_map<
            std::string,
            un::InputScope
        > const kStringIdsToLessonTypes = {
            std::make_pair("global", un::InputScope::eGlobal),
            std::make_pair("pipeline", un::InputScope::ePipeline),
            std::make_pair("instance", un::InputScope::eInstance)
        };

    }
    class Input {
    private:
        std::string name;
        std::string type;
    public:
        Input(const std::string& name, const std::string& type);

        const std::string& getName() const;

        const std::string& getType() const;
    };
}
#endif //UNNECESSARYENGINE_INPUTS_H
