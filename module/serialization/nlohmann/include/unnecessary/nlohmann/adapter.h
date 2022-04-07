#ifndef UNNECESSARYENGINE_ADAPTER_H
#define UNNECESSARYENGINE_ADAPTER_H

#include <nlohmann/json.hpp>
#include <unnecessary/serialization/serialized.h>
#include <unnecessary/archives/archiver.h>

namespace un::serialization {
    nlohmann::json adapt(const un::Serialized& serialized);
}
#endif