#include "unnecessary/serialization/serialized.h"

namespace un {
    SerializedType Serialized::getType() {
        return un::SerializedType::eComplex;
    }

    void Serialized::set(const std::string& key, const std::shared_ptr<un::SerializedNode>& result) {
        namedProperties[key] = result;
    }

    void Serialized::set(const uuids::uuid& key, const std::shared_ptr<un::SerializedNode>& result) {
        uuidProperties[key] = result;
    }

    const std::unordered_map<std::string, std::shared_ptr<un::SerializedNode>>& Serialized::getNamedProperties() const {
        return namedProperties;
    }

    const std::unordered_map<uuids::uuid, std::shared_ptr<un::SerializedNode>>& Serialized::getUUIDProperties() const {
        return uuidProperties;
    }
}