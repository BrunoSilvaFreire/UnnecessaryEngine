#ifndef UNNECESSARYENGINE_SERIALIZED_ARRAY_H
#define UNNECESSARYENGINE_SERIALIZED_ARRAY_H

#include <vector>
#include "serialization_types.h"

namespace un {
    template<typename TValue>
    class serialized_array : public serialized_node {
    private:
        serialized_type _elementsType;
        std::vector<TValue> _elements;

    public:
        void add(TValue&& entry) {
            _elements.emplace_back(std::move(entry));
        }

        operator std::vector<TValue>() {
            return _elements;
        };

        TValue& add() {
            return _elements.emplace_back();
        }

        serialized_type get_elements_type() const {
            return _elementsType;
        }

        serialized_type get_type() override {
            return array;
        }
    };
};
#endif
