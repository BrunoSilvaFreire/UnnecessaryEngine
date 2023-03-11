#ifndef UNNECESSARYENGINE_SERIALIZED_PRIMITIVE_H
#define UNNECESSARYENGINE_SERIALIZED_PRIMITIVE_H

#include "unnecessary/def.h"
#include "serialization_types.h"

namespace un {
    template<typename t_value>
    class serialized_primitive : public serialized_node {
    private:
        t_value _value;

    public:
        explicit serialized_primitive(t_value value) : _value(value) {
        }

        operator t_value() const {
            return _value;
        }
    };

    class serialized_byte : public serialized_primitive<u8> {
    public:
        serialized_byte(u8 value);

        serialized_type get_type() override;
    };

    class serialized_short : public serialized_primitive<u16> {
    public:
        serialized_short(u16 value);

        serialized_type get_type() override;
    };

    class serialized_integer : public serialized_primitive<u32> {
    public:
        serialized_integer(u32 value);

        serialized_type get_type() override;
    };

    class serialized_long : public serialized_primitive<u64> {
    public:
        serialized_long(u64 value);

        serialized_type get_type() override;
    };

    class serialized_float : public serialized_primitive<float> {
    public:
        serialized_float(float value);

        serialized_type get_type() override;
    };

    class serialized_double : public serialized_primitive<double> {
    public:
        serialized_double(double value);

    private:
        serialized_type get_type() override;
    };
}
#endif
