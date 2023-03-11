#ifndef UNNECESSARYENGINE_SERIALIZATION_TYPES_H
#define UNNECESSARYENGINE_SERIALIZATION_TYPES_H

#include <stdexcept>

namespace un {
    enum serialized_type {
        integer8,
        integer16,
        integer32,
        integer64,
        float_single,
        float_double,
        string,
        complex,
        array,
        binary
    };

    class serialized_node {
    public:
        virtual serialized_type get_type() = 0;

        template<typename TNode>
        TNode& as() {
            auto node = dynamic_cast<TNode*>(this);
            if (node == nullptr) {
                throw std::runtime_error("Invalid cast.");
            }
            return *node;
        }
    };
}

#endif
