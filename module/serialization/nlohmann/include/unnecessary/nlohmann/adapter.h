#ifndef UNNECESSARYENGINE_ADAPTER_H
#define UNNECESSARYENGINE_ADAPTER_H

#include <nlohmann/json.hpp>
#include <unnecessary/serialization/serialized.h>
#include <unnecessary/archives/archiver.h>

namespace un::serialization {
    nlohmann::json adapt(const un::Serialized& serialized);

    void adapt(const nlohmann::json& json, un::Serialized& serialized);

    template<typename TElement>
    un::SerializedArray<TElement>&& adapt_array(const nlohmann::json& json) {
        un::SerializedArray<TElement> arr;
        for (const auto& [key, value] : json.items()) {
            arr.add(static_cast<TElement>(value));
        }
        return std::move(arr);
    }

    template<>
    un::SerializedArray<un::Serialized>&& adapt_array(const nlohmann::json& json);
}

#endif