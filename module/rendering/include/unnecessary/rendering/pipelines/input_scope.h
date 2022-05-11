#ifndef UNNECESSARYENGINE_INPUT_SCOPE_H
#define UNNECESSARYENGINE_INPUT_SCOPE_H

#include <string>
#include <unordered_map>
#include <unnecessary/strings.h>

namespace un {

    enum [[un::serialize]] InputScope {
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
}
#endif //UNNECESSARYENGINE_INPUT_SCOPE_H
