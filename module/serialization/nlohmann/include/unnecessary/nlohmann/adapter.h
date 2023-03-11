#ifndef UNNECESSARYENGINE_ADAPTER_H
#define UNNECESSARYENGINE_ADAPTER_H

#include <nlohmann/json.hpp>
#include <unnecessary/serialization/serialized.h>
#include <unnecessary/archives/archiver.h>

namespace un::serialization {
    nlohmann::json adapt(const serialized& serialized);

    void adapt(const nlohmann::json& json, serialized& serialized);

    template<typename TElement>
    serialized_array<TElement>&& adapt_array(const nlohmann::json& json) {
        serialized_array<TElement> arr;
        for (const auto& [key, value] : json.items()) {
            arr.add(static_cast<TElement>(value));
        }
        return std::move(arr);
    }

    template<>
    serialized_array<serialized>&& adapt_array(const nlohmann::json& json);
}

#endif
