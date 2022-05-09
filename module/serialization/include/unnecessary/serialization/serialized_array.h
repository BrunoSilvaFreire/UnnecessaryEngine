#ifndef UNNECESSARYENGINE_SERIALIZED_ARRAY_H
#define UNNECESSARYENGINE_SERIALIZED_ARRAY_H

#include <vector>
#include "serialization_types.h"

namespace un {
    template<typename TValue>
    class SerializedArray : public SerializedNode {
    private:
        un::SerializedType elementsType;
        std::vector<TValue> elements;
    public:
        void add(TValue&& entry) {
            elements.emplace_back(std::move(entry));
        }

        operator std::vector<TValue>() {
            return elements;
        };

        TValue& add() {
            return elements.emplace_back();
        }

        un::SerializedType getElementsType() const {
            return elementsType;
        }

        un::SerializedType getType() override {
            return un::SerializedType::eArray;
        }
    };

};
#endif
