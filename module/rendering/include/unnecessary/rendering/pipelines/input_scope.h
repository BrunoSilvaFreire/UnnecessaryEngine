#ifndef UNNECESSARYENGINE_INPUT_SCOPE_H
#define UNNECESSARYENGINE_INPUT_SCOPE_H

#include <string>
#include <unordered_map>
#include <unnecessary/strings.h>

namespace un {
    enum [[un::generate_strings]] input_scope {
        /**
         * This input is only set once, at the start of the frame
         */

        global,
        /**
         * This input is set everytime the pipeline is bound
         */
        pipeline,
        /**
         * This input is set once per draw instance
         */
        instance
    };

    template<>
    std::string to_string(const input_scope& scope);

    namespace inputs {
        const std::unordered_map<std::string, input_scope> kStringIdsToInputScope = {
            std::make_pair("global", global),
            std::make_pair("pipeline", pipeline),
            std::make_pair("instance", instance)
        };
    }
}
#endif //UNNECESSARYENGINE_INPUT_SCOPE_H
