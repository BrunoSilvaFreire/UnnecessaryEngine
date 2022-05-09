
#ifndef UNNECESSARYENGINE_MAPS_H
#define UNNECESSARYENGINE_MAPS_H

#include <unordered_map>

namespace un::maps {
    template<typename TKey, typename TValue>
    const TValue* search(const std::unordered_map<TKey, TValue>& map, const TKey& key) {
        auto it = map.find(key);
        if (it == map.end()) {
            return nullptr;
        }
        return &it->second;
    }

    template<typename TKey, typename TValue>
    bool search(const std::unordered_map<TKey, TValue>& map, const TKey& key, TValue& result) {
        auto r = search(map, key);
        bool found = r != nullptr;
        if (found) {
            result = *r;
        }
        return found;
    }

}
#endif
