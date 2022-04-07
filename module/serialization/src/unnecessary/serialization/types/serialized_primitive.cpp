#include "unnecessary/serialization/serialized_primitive.h"

namespace un {

    SerializedByte::SerializedByte(u8 value) : SerializedPrimitive(value) { }

    un::SerializedType SerializedByte::getType() {
        return un::SerializedType::eInteger8;
    }

    SerializedShort::SerializedShort(u16 value) : SerializedPrimitive(value) { }

    un::SerializedType SerializedShort::getType() {
        return un::SerializedType::eInteger16;
    }

    SerializedInteger::SerializedInteger(u32 value) : SerializedPrimitive(value) { }

    un::SerializedType SerializedInteger::getType() {
        return un::SerializedType::eInteger32;
    }

    SerializedLong::SerializedLong(u64 value) : SerializedPrimitive(value) { }

    un::SerializedType SerializedLong::getType() {
        return un::SerializedType::eInteger64;
    }

    SerializedFloat::SerializedFloat(float value) : SerializedPrimitive(value) { }

    SerializedType SerializedFloat::getType() {
        return un::SerializedType::eFloat;
    }

    SerializedDouble::SerializedDouble(double value) : SerializedPrimitive(value) { }

    SerializedType SerializedDouble::getType() {
        return un::SerializedType::eDouble;
    }
}