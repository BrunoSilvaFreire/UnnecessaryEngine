#include <unnecessary/nlohmann/adapter.h>

namespace un::serialization {
    nlohmann::json adapt(const serialized& serialized) {
        nlohmann::json obj;
        for (const auto& [key, value] : serialized.get_named_properties()) {
            serialized_type type = value->get_type();
            switch (type) {
                case integer8:
                    obj[key] = un::serialization::read_value<u8>(value);
                    break;
                case integer16:
                    obj[key] = un::serialization::read_value<u16>(value);
                    break;
                case integer32:
                    obj[key] = un::serialization::read_value<u32>(value);
                    break;
                case integer64:
                    obj[key] = un::serialization::read_value<u64>(value);
                    break;
                case float_single:
                    obj[key] = un::serialization::read_value<f32>(value);
                    break;
                case float_double:
                    obj[key] = un::serialization::read_value<f64>(value);
                    break;
                case string:
                    obj[key] = un::serialization::read_value<std::string>(value);
                    break;
                case complex:
                    obj[key] = adapt(*std::static_pointer_cast<serialized>(value));
                    break;
                case array: {
                }
                    break;
                case binary:
                    break;
            }
        }
        return obj;
    }

    template<typename TElement>
    void read_arr_into(const nlohmann::json& jsonArr, std::string key, serialized& serialized) {
        serialized_array<TElement> arr = std::move(adapt_array<TElement>(jsonArr));
        serialized.set(key, std::move(arr));
    }

    void adapt_arr(const nlohmann::json& jsonArr, std::string key, serialized& serialized) {
        switch (jsonArr.begin()->type()) {
            case nlohmann::json::value_t::object:
                return read_arr_into<serialized>(jsonArr, key, serialized);
            case nlohmann::json::value_t::string:
                return read_arr_into<std::string>(jsonArr, key, serialized);
            case nlohmann::json::value_t::boolean:
                return read_arr_into<bool>(jsonArr, key, serialized);
            case nlohmann::json::value_t::number_integer:
                return read_arr_into<i32>(jsonArr, key, serialized);
            case nlohmann::json::value_t::number_unsigned:
                return read_arr_into<u32>(jsonArr, key, serialized);
            case nlohmann::json::value_t::number_float:
                return read_arr_into<f32>(jsonArr, key, serialized);
            default:
                break;
        }
    }

    void adapt(const nlohmann::json& json, serialized& serialized) {
        for (const auto& [key, item] : json.items()) {
            switch (item.type()) {
                case nlohmann::json::value_t::object: {
                    serialized obj;
                    adapt(item, obj);
                    serialized.set(key, obj);
                }
                    break;
                case nlohmann::json::value_t::array:
                    adapt_arr(item, key, serialized);
                    break;
                case nlohmann::json::value_t::string:
                    serialized.set(key, static_cast<std::string>(item));
                    break;
                case nlohmann::json::value_t::boolean:
                    serialized.set(key, static_cast<bool>(item));
                    break;
                case nlohmann::json::value_t::number_integer:
                    serialized.set(key, static_cast<u32>(item));
                    break;
                case nlohmann::json::value_t::number_unsigned:
                    serialized.set(key, static_cast<int>(item));
                    break;
                case nlohmann::json::value_t::number_float:
                    serialized.set(key, static_cast<f32>(item));
                    break;
                case nlohmann::json::value_t::binary:
                    break;
                default:
                    break;
            }
        }
    }

    template<>
    serialized_array<serialized>&& adapt_array(const nlohmann::json& json) {
        serialized_array<serialized> arr;
        for (const auto& item : json.items()) {
            serialized s;
            adapt(json, s);
            arr.add(std::move(s));
        }
        return std::move(arr);
    }
}
