#include <unnecessary/nlohmann/adapter.h>

namespace un::serialization {

    nlohmann::json adapt(const Serialized& serialized) {
        nlohmann::json obj;
        for (const auto& [key, value] : serialized.getNamedProperties()) {

            un::SerializedType type = value->getType();
            switch (type) {
                case eInteger8:
                    obj[key] = un::serialization::read_value<u8>(value);
                    break;
                case eInteger16:
                    obj[key] = un::serialization::read_value<u16>(value);
                    break;
                case eInteger32:
                    obj[key] = un::serialization::read_value<u32>(value);
                    break;
                case eInteger64:
                    obj[key] = un::serialization::read_value<u64>(value);
                    break;
                case eFloat:
                    obj[key] = un::serialization::read_value<f32>(value);
                    break;
                case eDouble:
                    obj[key] = un::serialization::read_value<f64>(value);
                    break;
                case eString:
                    obj[key] = un::serialization::read_value<std::string>(value);
                    break;
                case eComplex:
                    obj[key] = adapt(*std::static_pointer_cast<un::Serialized>(value));
                    break;
                case eArray:
                    break;
                case eBinary:
                    break;
            }
        }
        return obj;
    }
}