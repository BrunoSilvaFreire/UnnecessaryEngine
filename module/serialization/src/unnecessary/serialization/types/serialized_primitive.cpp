#include "unnecessary/serialization/serialized_primitive.h"

namespace un {
    serialized_byte::serialized_byte(u8 value) : serialized_primitive(value) {
    }

    serialized_type serialized_byte::get_type() {
        return integer8;
    }

    serialized_short::serialized_short(u16 value) : serialized_primitive(value) {
    }

    serialized_type serialized_short::get_type() {
        return integer16;
    }

    serialized_integer::serialized_integer(u32 value) : serialized_primitive(value) {
    }

    serialized_type serialized_integer::get_type() {
        return integer32;
    }

    serialized_long::serialized_long(u64 value) : serialized_primitive(value) {
    }

    serialized_type serialized_long::get_type() {
        return integer64;
    }

    serialized_float::serialized_float(float value) : serialized_primitive(value) {
    }

    serialized_type serialized_float::get_type() {
        return float_single;
    }

    serialized_double::serialized_double(double value) : serialized_primitive(value) {
    }

    serialized_type serialized_double::get_type() {
        return float_double;
    }
}
