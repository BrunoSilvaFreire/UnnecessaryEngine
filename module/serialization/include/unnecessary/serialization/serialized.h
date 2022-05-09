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
    class Serialized;

    namespace serialization {
        template<>
        struct TypeTranslation<un::Serialized> {
            typedef un::Serialized SerializedType;
        };
    }

    class Serialized : public SerializedNode {
    private:
        std::unordered_map<std::string, std::shared_ptr<un::SerializedNode>> namedProperties;
        std::unordered_map<uuids::uuid, std::shared_ptr<un::SerializedNode>> uuidProperties;
    public:
        SerializedType getType() override;

        const std::unordered_map<std::string, std::shared_ptr<un::SerializedNode>>& getNamedProperties() const;

        const std::unordered_map<uuids::uuid, std::shared_ptr<un::SerializedNode>>& getUUIDProperties() const;

        void set(const std::string& key, const std::shared_ptr<un::SerializedNode>& result);

        void set(const uuids::uuid& key, const std::shared_ptr<un::SerializedNode>& result);

        template<typename TValue>
        void set(const std::string& key, const TValue& result) {
            auto node = un::serialization::create_node<TValue>(result);
            auto sharedPtr = std::make_shared<decltype(node)>(node);
            namedProperties[key] = std::static_pointer_cast<un::SerializedNode>(sharedPtr);
        }

        template<typename TValue>
        void set(const std::string& key, SerializedArray<TValue>&& arr) {
            auto sharedPtr = std::make_shared<SerializedArray<TValue>>(std::move(arr));
            namedProperties[key] = std::static_pointer_cast<un::SerializedNode>(sharedPtr);
        }

        template<typename TValue>
        void set(const uuids::uuid& key, const TValue& result) {
            auto node = un::serialization::create_node<TValue>(result);
            auto sharedPtr = std::make_shared<decltype(node)>(node);
            uuidProperties[key] = std::static_pointer_cast<un::SerializedNode>(sharedPtr);
        }

        template<typename T>
        bool try_get_node(const std::string& key, std::shared_ptr<T>& result) const {
            const auto& it = namedProperties.find(key);
            if (it != namedProperties.end()) {
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
            using SerializedType = typename un::serialization::TypeTranslation<T>::SerializedType;
            std::shared_ptr<SerializedType> ptr;
            if (try_get_node<SerializedType>(key, ptr)) {
                result = static_cast<T>(*ptr);
                return true;
            }
            return false;
        }
        template<typename T>
        bool try_get(const std::string& key, un::SerializedArray<T>& result) const {
            std::shared_ptr<un::SerializedArray<T>> ptr;
            if (try_get_node<un::SerializedArray<T>>(key, ptr)) {
                result = *ptr;
                return true;
            }
            return false;
        }
    };

    template<>
    void Serialized::set<bool>(const std::string& key, const bool& value) {
        set(key, static_cast<u8>(value));
    }

    template<>
    void Serialized::set<bool>(const uuids::uuid& key, const bool& value) {
        set(key, static_cast<u8>(value));
    }

    template<>
    bool Serialized::try_get<bool>(const std::string& key, bool& result) const {
        try_get(key, reinterpret_cast<u8&>(result));
    }
}
#endif
