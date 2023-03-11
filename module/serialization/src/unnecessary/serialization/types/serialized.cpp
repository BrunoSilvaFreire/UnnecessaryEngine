#include "unnecessary/serialization/serialized.h"

namespace un {
    serialized_type serialized::get_type() {
        return complex;
    }

    void serialized::set(const std::string& key, const std::shared_ptr<serialized_node>& result) {
        _namedProperties[key] = result;
    }

    void serialized::set(const uuids::uuid& key, const std::shared_ptr<serialized_node>& result) {
        _uuidProperties[key] = result;
    }

    const std::unordered_map<std::string, std::shared_ptr<serialized_node>>&
    serialized::get_named_properties() const {
        return _namedProperties;
    }

    const std::unordered_map<uuids::uuid, std::shared_ptr<serialized_node>>&
    serialized::get_uuid_properties() const {
        return _uuidProperties;
    }
}
