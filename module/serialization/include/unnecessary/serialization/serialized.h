#ifndef UNNECESSARYENGINE_SERIALIZED_H
#define UNNECESSARYENGINE_SERIALIZED_H

#include "serialized_string.h"
#include "serialized_array.h"
#include "type_translation.h"
#include <unordered_map>
#include <string>
#include <memory>
#include <uuid.h>

namespace un {
    class serialized;

    namespace serialization {
        template<>
        struct type_translation<serialized> {
            using serialized_type = serialized;
        };
    }

    class serialized : public serialized_node {
    private:
        std::unordered_map<std::string, std::shared_ptr<serialized_node>> _namedProperties;
        std::unordered_map<uuids::uuid, std::shared_ptr<serialized_node>> _uuidProperties;
    public:
        serialized_type get_type() override;

        const std::unordered_map<std::string, std::shared_ptr<serialized_node>>&
        get_named_properties() const;

        const std::unordered_map<uuids::uuid, std::shared_ptr<serialized_node>>&
        get_uuid_properties() const;

        void set(const std::string& key, const std::shared_ptr<serialized_node>& result);

        void set(const uuids::uuid& key, const std::shared_ptr<serialized_node>& result);

        template<typename t_value>
        void set(const std::string& key, const t_value& result) {
            auto node = serialization::create_node<t_value>(result);
            auto sharedPtr = std::make_shared<decltype(node)>(node);
            _namedProperties[key] = std::static_pointer_cast<serialized_node>(sharedPtr);
        }

        template<typename TValue>
        void set(const std::string& key, serialized_array<TValue>&& arr) {
            auto sharedPtr = std::make_shared<serialized_array<TValue>>(std::move(arr));
            _namedProperties[key] = std::static_pointer_cast<serialized_node>(sharedPtr);
        }

        template<typename TValue>
        void set(const uuids::uuid& key, const TValue& result) {
            auto node = serialization::create_node<TValue>(result);
            auto sharedPtr = std::make_shared<decltype(node)>(node);
            _uuidProperties[key] = std::static_pointer_cast<serialized_node>(sharedPtr);
        }

        template<typename T>
        bool try_get_node(const std::string& key, std::shared_ptr<T>& result) const {
            const auto& it = _namedProperties.find(key);
            if (it != _namedProperties.end()) {
                auto casted = std::dynamic_pointer_cast<T>(it->second);
                if (casted != nullptr) {
                    result = casted;
                    return true;
                }
            }
            result = nullptr;
            return false;
        }

        template<typename T>
        bool try_get(const std::string& key, T& result) const {
            using serialized_type = typename serialization::type_translation<T>::serialized_type;
            std::shared_ptr<serialized_type> ptr;
            if (try_get_node<serialized_type>(key, ptr)) {
                result = static_cast<T>(*ptr);
                return true;
            }
            return false;
        }

        template<typename T>
        bool try_get(const std::string& key, serialized_array<T>& result) const {
            std::shared_ptr<serialized_array<T>> ptr;
            if (try_get_node<serialized_array<T>>(key, ptr)) {
                result = *ptr;
                return true;
            }
            return false;
        }
    };

    template<>
    inline void serialized::set<bool>(const std::string& key, const bool& value) {
        set(key, static_cast<u8>(value));
    }

    template<>
    inline void serialized::set<bool>(const uuids::uuid& key, const bool& value) {
        set(key, static_cast<u8>(value));
    }

    template<>
    inline void serialized::set<i32>(const std::string& key, const i32& value) {
        set(key, *reinterpret_cast<const u32*>(&value));
    }

    template<>
    inline void serialized::set<i32>(const uuids::uuid& key, const i32& value) {
        set(key, *reinterpret_cast<const u32*>(&value));
    }

    template<>
    inline bool serialized::try_get<bool>(const std::string& key, bool& result) const {
        return try_get(key, reinterpret_cast<u8&>(result));
    }
}
#endif
